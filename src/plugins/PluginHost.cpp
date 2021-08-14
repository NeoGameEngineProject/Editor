#include "PluginHost.h"
#include "mainwindow.h"

#include <platform/EditorWidget.h>

#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QMenu>

#include <lqt-external/common/lqt_common.hpp>

#include <filesystem>

namespace fs = std::filesystem;

#include <Log.h>

using namespace Neo;

static QMenu* findSubMenu(QMenu* parent, const QString& name)
{
	if(parent->menuAction()->text().remove("&") == name)
		return parent;
	
	for(auto* m : parent->findChildren<QMenu*>())
	{
		if(m->menuAction()->text().remove("&") == name)
			return m;
	}

	return nullptr;
}

static QMenu* findMenu(QMenu* parent, const char* path)
{
	QString str(path);
	auto parts = str.split('/');

	QMenu* iter = parent;
	int i = 0;
	do
	{
		iter = findSubMenu(iter, parts[i++]);
	} while (iter && i < parts.size());

	return iter;
}


PluginHost& PluginHost::get()
{
	static PluginHost p;
	return p;
}

namespace LuaBindings
{
static int GetEditorLevel(lua_State* L)
{
	auto* win = PluginHost::get().getWindow();
	pushLevel(L, &win->getEditorLevel());
	return 1;
}

static int GetEditorCamera(lua_State* L)
{
	auto* win = PluginHost::get().getWindow();
	pushCameraBehavior(L, &win->getEditorCamera());
	return 1;
}

static int GetEditorSelection(lua_State* L)
{
	auto* win = PluginHost::get().getWindow();
	auto* editor = win->getEditor();

	auto& selection = editor->getSelection();
	lua_newtable(L);

	for(int i = 0; i < selection.size(); i++)
	{
		lua_pushinteger(L, i+1);

		// FIXME Should use handles instead as pointers may become
		// invalid!
		// FIXME: Constness!!!!!
		pushObject(L, (Neo::Object*) selection[i].get());
		lua_rawset(L, -3);
	}
	
	return 1;
}

static int GetEditorMovementSpeed(lua_State* L)
{
	auto* win = PluginHost::get().getWindow();
	auto* editor = win->getEditor();

	lua_pushnumber(L, editor->getMovementSpeed());
	
	return 1;
}
}

void PluginHost::load(const std::string& file)
{
	auto P = std::make_shared<LuaScript>();
	auto* L = P->getState();

	m_plugins.push_back(P);

	lua_register(L, "GetEditorLevel", LuaBindings::GetEditorLevel);
	lua_register(L, "GetEditorCamera", LuaBindings::GetEditorCamera);
	lua_register(L, "GetEditorSelection", LuaBindings::GetEditorSelection);
	lua_register(L, "GetEditorMovementSpeed", LuaBindings::GetEditorMovementSpeed);

	lua_register(L, "AddMenuAction", [](lua_State* L) -> int
	{
		int top = lua_gettop(L);
		if(top != 2 || !lua_isstring(L, 1) || !lqtL_isudata(L, 2, "QAction*"))
		{
			lua_settop(L, top);
			return 0;
		}

		auto* parentMenuPath = lua_tostring(L, 1);
		QAction* action = static_cast<QAction*>(lqtL_toudata(L, 2, "QAction*"));
  		lqtL_selfcheck(L, action, "QAction");

		auto& h = PluginHost::get();
		QMainWindow* win = h.getWindow();

		if(strlen(parentMenuPath) == 0)
		{
			win->menuBar()->addAction(action);
			lua_settop(L, top);
			return 0;
		}

		bool success = false;
		for(auto* m : win->menuBar()->findChildren<QMenu*>())
		{
			auto* submenu = findMenu(m, parentMenuPath);
			if(submenu)
			{
				success = true;
				submenu->addAction(action);
				break;
			}
		}

		if(!success)
			LOG_ERROR("Could not register menu action at " << parentMenuPath);

		lua_settop(L, top);
		return 0;
	});

	lua_register(L, "AddMenu", [](lua_State* L) -> int
	{
		int top = lua_gettop(L);

		if(top != 2 || !lua_isstring(L, 1) || !lqtL_isudata(L, 2, "QMenu*"))
		{
			lua_settop(L, top);
			return 0;
		}

		auto* parentMenuPath = lua_tostring(L, 1);
		QMenu* menu = static_cast<QMenu*>(lqtL_toudata(L, 2, "QMenu*"));
  		lqtL_selfcheck(L, menu, "QMenu");

		auto& h = PluginHost::get();
		QMainWindow* win = h.getWindow();

		if(strlen(parentMenuPath) == 0)
		{
			win->menuBar()->addMenu(menu);
			lua_settop(L, top);
			return 0;
		}

		bool success = false;
		for(auto* m : win->menuBar()->findChildren<QMenu*>())
		{
			auto* submenu = findMenu(m, parentMenuPath);
			if(submenu)
			{
				success = true;
				submenu->addMenu(menu);
				break;
			}
		}

		if(!success)
			LOG_ERROR("Could not register menu at " << parentMenuPath);

		lua_settop(L, top);
		return 0;
	});

	lua_register(L, "AddToolbarAction", [](lua_State* L) -> int
	{
		int top = lua_gettop(L);
		if(top != 1 || !lqtL_isudata(L, 1, "QAction*"))
		{
			lua_settop(L, top);
			return 0;
		}

		QAction* action = static_cast<QAction*>(lqtL_toudata(L, 1, "QAction*"));
  		lqtL_selfcheck(L, action, "QAction");

		auto& h = PluginHost::get();
		QMainWindow* win = h.getWindow();
		win->findChild<QToolBar*>()->addAction(action);

		lua_settop(L, top);
		return 0;
	});

	lua_register(L, "AddDockWidget", [](lua_State* L) -> int
	{
		int top = lua_gettop(L);
		if(top != 2 || !lqtL_isenum(L, 1, "Qt.DockWidgetArea") || !lqtL_isudata(L, 2, "QDockWidget*"))
		{
			LOG_WARNING("Could not register dock widget: Types don't match!")
			lua_settop(L, top);
			return 0;
		}

		auto area = (Qt::DockWidgetArea) (lqtL_toenum(L, 1, "Qt.DockWidgetArea"));

		QDockWidget* dock = static_cast<QDockWidget*>(lqtL_toudata(L, 2, "QDockWidget*"));
  		lqtL_selfcheck(L, dock, "QDockWidget");

		auto& h = PluginHost::get();
		QMainWindow* win = h.getWindow();
		win->addDockWidget(area, dock);

		lua_settop(L, top);
		return 0;
	});

	lua_register(L, "RegisterInputMethod", [](lua_State* L) -> int
	{
		auto& h = PluginHost::get();
		auto plugin = h.findPlugin(L);
		
		if(plugin)
			h.getWindow()->registerInputMethod(plugin);
		else
			LOG_ERROR("The currently running plugin does not seem to be registered! This should never happen!");

		return 0;
	});

	P->doString("package.path = package.path .. ';" + m_luaModulePath + "'");
	P->doString("package.cpath = package.cpath .. ';" + m_modulePath + "'");
	P->doFile(file.c_str());

	LOG_DEBUG("Loaded plugin: " << P->getGlobalString("PluginName") << " by " << P->getGlobalString("PluginAuthor"));
}

#ifdef __APPLE__ // No std::filesystem before Catalina, yay...
#include <dirent.h>
#include <sys/types.h>
#endif

void PluginHost::loadDirectory(const std::string& dir)
{
	addModulePath(dir);
#ifndef __APPLE__
	for(auto& p: fs::directory_iterator(dir))
	{
		if(p.is_directory())
			continue;

		LOG_DEBUG("Loading Lua plugin: " << p.path());
		load(p.path().string());
	}
#else
	DIR* dirptr = opendir(dir.c_str());
	struct dirent* entry = nullptr;

	if(!dirptr)
		return;

	while((entry = readdir(dirptr)) != nullptr)
	{
		if(entry->d_type & DT_DIR)
			continue;

		std::string path = dir + "/" + entry->d_name;
		LOG_DEBUG("Loading Lua plugin: " << path);
		load(path);
	}
	
	closedir(dirptr);
#endif
}

std::shared_ptr<Neo::LuaScript> PluginHost::findPlugin(lua_State* L)
{
	auto iter = std::find_if(m_plugins.begin(), m_plugins.end(), [L](const std::shared_ptr<Neo::LuaScript>& script) {
		return script->getState() == L;
	});

	if(iter == m_plugins.end())
		return nullptr;

	return *iter;
}
