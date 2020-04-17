#include "EditorWidget.h"
#include <Level.h>
#include <Platform.h>

#include <Log.h>

#include <imgui.h>
#include "imgui_impl_opengl3.h"

#include <ImGuizmo.h>

#include <QApplication>
#include <QDir>
#include <QDesktopWidget>

using namespace Neo;

EditorWidget::EditorWidget(QWidget* parent):
	LevelWidget(parent)
{

}

void EditorWidget::updateDPI()
{
	// devicePixelRatioF only returns 1.0 everytime. Choose the desktop config if it is bigger instead.
	const auto oldScale = m_dpiScale;
	m_dpiScale = std::max(devicePixelRatioF(), QApplication::desktop()->devicePixelRatioF());

	m_scaledWidth = width() * m_dpiScale;
	m_scaledHeight = height() * m_dpiScale;

	if(oldScale != m_dpiScale)
		LevelWidget::resizeGL(m_scaledWidth, m_scaledHeight);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplayFramebufferScale = ImVec2(m_dpiScale, m_dpiScale);
}

void EditorWidget::initializeGL()
{
	LevelWidget::initializeGL();
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	
	ImGui_ImplOpenGL3_Init("#version 150");
	
	ImGui::StyleColorsDark();
	ImGuizmo::SetRect(0, 0, width(), height());
	ImGuizmo::Enable(true);
	ImGuizmo::SetOrthographic(false);

	auto lvl = getLevel();
	m_objectTextures[0] = lvl->loadTexture((QApplication::applicationDirPath() + "/assets/light.png").toUtf8().data());
	m_objectTextures[1] = lvl->loadTexture((QApplication::applicationDirPath() + "/assets/camera.png").toUtf8().data());
	m_objectTextures[2] = lvl->loadTexture((QApplication::applicationDirPath() + "/assets/sound.png").toUtf8().data());

	auto render = getRenderer();
	m_objectTextures[0]->setID(render->createTexture(m_objectTextures[0]));
	m_objectTextures[1]->setID(render->createTexture(m_objectTextures[1]));
	m_objectTextures[2]->setID(render->createTexture(m_objectTextures[2]));
}

void EditorWidget::resizeGL(int w, int h)
{
	updateDPI();

	w *= m_dpiScale;
	h *= m_dpiScale;

	m_scaledWidth = w;
	m_scaledHeight = h;

	LevelWidget::resizeGL(w, h);
	
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = width();
	io.DisplaySize.y = height();
	
	ImGuizmo::SetRect(0, 0, width(), height());
}

void EditorWidget::updateImGuiInput()
{
	// Apply input!
	auto& input = getPlatform().getInputContext();
	auto& mouse = input.getMouse();
	
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(mouse.getPosition().x, mouse.getPosition().y);
	io.MouseDelta = ImVec2(mouse.getDirection().x, mouse.getDirection().y);
	
	io.MouseDown[0] = mouse.isKeyDown(MOUSE_BUTTON_LEFT);
	io.MouseDown[1] = mouse.isKeyDown(MOUSE_BUTTON_RIGHT);
	io.MouseDown[2] = mouse.isKeyDown(MOUSE_BUTTON_MIDDLE);
}

void EditorWidget::paintGL()
{
	endFrame();
	
	updateDPI();

	beginFrame();

	// Because in the init method, no defaultFramebufferObject is bound.
	getRenderer()->setBackbuffer((void*) defaultFramebufferObject());

	// Update selection
	auto& input = getPlatform().getInputContext();
	Vector2 mousepos = input.getMouse().getPosition();
	ObjectHandle selectedObject;

	// Saved here so we can query it when drawing lights etc.
	bool triggerSelect = input.getMouse().onKeyDown(MOUSE_BUTTON_LEFT);
	if(triggerSelect && !ImGuizmo::IsOver())
	{
		auto& camera = getCamera();

		Vector3 origin = camera.getParent()->getPosition();
		Vector3 direction = camera.getUnProjectedPoint(Vector3(mousepos.x*m_dpiScale, (height() - mousepos.y)*m_dpiScale, 1));
		direction = (direction - origin).getNormalized();

		Vector3 hit;
		if(getLevel()->castRay(origin, direction, 1000000.0f, &hit, &selectedObject))
		{
			if(input.isKeyDown(KEY_LSHIFT) || input.isKeyDown(KEY_RSHIFT))
			{
				m_selection.push_back(selectedObject);
			}
			else
			{
				m_selection.clear();
				m_selection.push_back(selectedObject);
			}

			emit objectSelectionChanged(selectedObject);
			emit objectSelectionListChanged(m_selection);
		}
		else if(!input.isKeyDown(KEY_LSHIFT) && !input.isKeyDown(KEY_RSHIFT))
		{
			m_selection.clear();

			emit objectSelectionChanged(ObjectHandle());
			emit objectSelectionListChanged(m_selection);
		}
	}

	// FIXME: HACK!
	input.flush();

	updateImGuiInput();
	
	Matrix4x4 id;
	id.loadIdentity();
	id.setRotationX(90);

	// First, render grid so everything is in front of it.
#if 0
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	// ImGuizmo::DrawGrid(getCamera().getViewMatrix().entries, getCamera().getProjectionMatrix().entries, id.entries, 10.f);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowBgAlpha(0);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width(), height()));
	
	ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoTitleBar
						| ImGuiWindowFlags_NoResize
						| ImGuiWindowFlags_NoScrollbar
						| ImGuiWindowFlags_NoInputs
						| ImGuiWindowFlags_NoSavedSettings
						| ImGuiWindowFlags_NoFocusOnAppearing
						| ImGuiWindowFlags_NoBringToFrontOnFocus);
	{
		auto lvl = getLevel();
		for(auto& obj : lvl->getObjects())
		{
			unsigned int icon;
			if(obj.getBehavior<LightBehavior>()) icon = 0;
			else if(obj.getBehavior<CameraBehavior>()) icon = 1;
			else if(obj.getBehavior<SoundBehavior>()) icon = 2;
			else continue;

			const float iconSize = 64;
			auto p = getCamera().getProjectedPoint(obj.getTransform().getTranslationPart()) / m_dpiScale;
			p.y = height() - p.y;

			p += Vector3(-iconSize/2, -iconSize/2, 0);

			// TODO Make icon size configurable!
			if(p.x <= width() + iconSize && p.y <= height() + iconSize && (p.z * m_dpiScale) < 1.0f)
			{
				// Icon * scale / distance TODO Make configurable
				float scale = iconSize; // * 10.0f /(obj.getPosition() - getCamera().getParent()->getPosition()).getLength();
				ImGui::SetCursorPos(ImVec2(p.x, p.y));
				ImGui::Image((ImTextureID) m_objectTextures[icon]->getID(), ImVec2(scale, scale));

				// Update selection
				if(triggerSelect
					&& mousepos.x >= p.x && mousepos.x < (p.x + iconSize)
					&& mousepos.y >= p.y && mousepos.y < (p.y + iconSize))
				{
					if(!selectedObject.empty())
						m_selection.pop_back();

					if(!input.isKeyDown(KEY_LSHIFT) && !input.isKeyDown(KEY_RSHIFT))
						m_selection.clear();

					m_selection.push_back(obj.getSelf());

					emit objectSelectionChanged(obj.getSelf());
					emit objectSelectionListChanged(m_selection);
				}
			}
		}
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin(tr("Statistics").toUtf8().data());
	ImGui::Text("Draw Calls: %d\n"
				"Triangles:  %d\n"
				"Frametime:  %f\n"
				"FPS:        %f",
				
				getRenderer()->getDrawCallCount(),
				getRenderer()->getFaceCount(),
				getDeltaTime(),
				1000.0f/getDeltaTime());
	ImGui::End();
	
	ImGuizmo::BeginFrame();
	if(!m_selection.empty())
	{
		ImGuizmo::Enable(true);
		auto obj = m_selection[0];
		
		ImGuizmo::OPERATION op;
		switch(m_mode)
		{
			case EDITOR_SCALE: op = ImGuizmo::OPERATION::SCALE; break;
			case EDITOR_ROTATE: op = ImGuizmo::OPERATION::ROTATE; break;
			case EDITOR_TRANSLATE:
			default:
				op = ImGuizmo::OPERATION::TRANSLATE; break;
		}
		
		if(m_selection.size() > 1)
		{
			static Matrix4x4 selection;
			selection.setTranslation(selectionCenter());
			
			Matrix4x4 delta;
			ImGuizmo::Manipulate(getCamera().getViewMatrix().entries, 
						getCamera().getProjectionMatrix().entries, 
						op, 
						ImGuizmo::MODE::WORLD, 
						selection.entries, 
						delta.entries,
						0, 
						NULL, 
						NULL);
			
			Matrix4x4 translation;
			translation.loadIdentity();
			translation.setTranslation(selection.getTranslationPart());
			
			Matrix4x4 invTranslation = translation.getInverse();
			
			Matrix4x4 rotation;
			rotation.loadIdentity();
			rotation.setRotationEuler(selection.getEulerAngles().x, selection.getEulerAngles().y, selection.getEulerAngles().z);
			
			for(auto& object : m_selection)
			{
				if(m_mode == EDITOR_ROTATE)
				{
					object->getTransform() = translation * rotation * invTranslation * object->getTransform();
					object->updateFromMatrix();
				}
				else if(m_mode == EDITOR_TRANSLATE)
				{
					object->translate(delta.getTranslationPart());
				}
				else
				{
					Vector3 scale = (selection.getScale() - Vector3(1, 1, 1));
					object->setScale(object->getScale() + scale);
					
					Vector3 translationAxis = (object->getGlobalPosition() - selection.getTranslationPart()).getNormalized();
					object->setPosition(object->getGlobalPosition() + (translationAxis * scale * 5.0f));
				}

				#if 0
				Matrix4x4 localRotation = translation * rotation * invTranslation; // * object->getTransform();
				object->getTransform() = localRotation * object->getTransform(); //(rotation.getInverse() * delta) * localRotation;

				if(!object->getParent().empty())
					object->getTransform() = object->getParent()->getTransform().getInverse() * object->getTransform();

				object->updateFromMatrix();

				Vector3 scale = (selection.getScale() - Vector3(1, 1, 1));
				object->setScale(object->getScale() + scale*0.1f);
				object->translate(delta.getTranslationPart());
				#endif

				object->makeSubtreeDirty();
				object->updateMatrix();
			}
		}
		else
		{
			Matrix4x4 delta;
			ImGuizmo::Manipulate(getCamera().getViewMatrix().entries, 
						getCamera().getProjectionMatrix().entries, 
						op, 
						ImGuizmo::MODE::WORLD, 
						obj->getTransform().entries, 
						delta.entries,
						0, 
						NULL, 
						NULL);

			if(delta != Matrix4x4())
			{
				if(!obj->getParent().empty())
					obj->getTransform() = obj->getParent()->getTransform().getInverse() * obj->getTransform();

				obj->updateFromMatrix();
				obj->makeSubtreeDirty();
				emit objectChanged(obj);
			}
		}
	}
	else
	{
		ImGuizmo::Enable(false);
	}

	LevelWidget::paintGL();
	ImGui::Render();
	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Clear alpha buffer so we don't blend with the desktop wallpaper
	glColorMask(false, false, false, true);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glColorMask(true, true, true, true);

	glFinish();
}

void EditorWidget::setSelection(const std::vector<ObjectHandle>& selection)
{
	m_selection = selection;
}

Vector3 EditorWidget::selectionCenter()
{
	Vector3 center;
	for(auto& object : m_selection)
	{
		center += object->getPosition();
	}
	
	center /= m_selection.size();
	return center;
}

void EditorWidget::makePathsRelative(const std::string& dir)
{
	QDir d(dir.c_str());
	auto* level = getLevel().get();

	if(!level) return;

	const std::function<void(Object*)> fn = [&d, &fn](Object* o)
	{
		for(auto& b : o->getBehaviors())
		{
			for(auto* p : b->getProperties())
			{
				if(p->getType() == PATH)
				{
					p->set<std::string>(d.relativeFilePath(p->get<std::string>().c_str()).toStdString());
				}
			}
		}

		for(auto& c : o->getChildren())
			fn(c.get());
	};

	fn(level->getRoot().get());
}

