#ifndef __PLUGIN_HOST_H__
#define __PLUGIN_HOST_H__

#include <LuaScript.h>
#include <vector>
#include <QMainWindow>

class MainWindow;

class PluginHost
{
public:
	void load(const std::string& file);
	void loadDirectory(const std::string& dir);
	void addModulePath(const std::string& str)
	{
		m_luaModulePath += ";" + str + "?.lua;";
		m_modulePath += ";"
			+ str + "?.so;"
			+ str + "?.dylib;"
			+ str + "?.dll";
	}

	MainWindow* getWindow() { return m_window; }
	void setWindow(MainWindow* window) { m_window = window; }

	std::vector<Neo::LuaScript>& getPlugins() { return m_plugins; }

	static PluginHost& get();

private:
	std::vector<Neo::LuaScript> m_plugins;
	std::string m_modulePath, m_luaModulePath;
	MainWindow* m_window = nullptr;
};

#endif
