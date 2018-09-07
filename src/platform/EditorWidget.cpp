#include "EditorWidget.h"
#include <Level.h>
#include <Platform.h>

#include <Log.h>

#include <imgui.h>
#include "imgui_impl_opengl3.h"

#include <ImGuizmo.h>

using namespace Neo;

EditorWidget::EditorWidget(QWidget* parent):
	LevelWidget(parent)
{

}

void EditorWidget::initializeGL()
{
	LevelWidget::initializeGL();
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	
	io.DisplaySize.x = width();
	io.DisplaySize.y = height();
	
	ImGui_ImplOpenGL3_Init("#version 150");
	
	ImGui::StyleColorsDark();
	
	ImGuizmo::SetRect(0, 0, width(), height());
	ImGuizmo::Enable(true);
	ImGuizmo::SetOrthographic(false);
}

void EditorWidget::resizeGL(int w, int h)
{
	LevelWidget::resizeGL(w, h);
	
	ImGuiIO& io = ImGui::GetIO();
	
	io.DisplaySize.x = width();
	io.DisplaySize.y = height();
	
	ImGuizmo::SetRect(0, 0, w, h);
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
	LevelWidget::paintGL();
	
	// Update selection
	auto& input = getPlatform().getInputContext();
	if(input.getMouse().onKeyDown(MOUSE_BUTTON_LEFT) && !ImGuizmo::IsOver())
	{
		auto& camera = getCamera();
		
		Vector2 mousepos = input.getMouse().getPosition();
		Vector3 origin = camera.getParent()->getPosition();
		Vector3 direction = camera.getUnProjectedPoint(Vector3(mousepos.x, height() - mousepos.y, 1));
		direction = (direction - origin).getNormalized();
	
		Vector3 hit;
		ObjectHandle object;
		if(getLevel()->castRay(origin, direction, 1000000.0f, &hit, &object))
		{
			if(input.isKeyDown(KEY_LSHIFT) || input.isKeyDown(KEY_RSHIFT))
			{
				m_selection.push_back(object);
			}
			else
			{
				m_selection.clear();
				m_selection.push_back(object);
			}
			
			emit objectSelectionChanged(object);
			emit objectSelectionListChanged(m_selection);
		}
		else
		{
			m_selection.clear();
			
			emit objectSelectionChanged(ObjectHandle());
			emit objectSelectionListChanged(m_selection);
		}
		
		// FIXME: HACK!
		input.flush();
	}
	
	updateImGuiInput();
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
	
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin(tr("Statistics").toUtf8().data());
	ImGui::Text("Statistics");
	ImGui::End();
	
	ImGuizmo::BeginFrame();
	
	Matrix4x4 id;
	id.loadIdentity();
	id.setRotationX(90);
	
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
		
		// ImGuizmo::DrawGrid(getCamera().getViewMatrix().entries, getCamera().getProjectionMatrix().entries, id.entries, 10.f);
		
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
					object->updateMatrix();
				}
				else
				{
					Vector3 scale = (selection.getScale() - Vector3(1, 1, 1));
					object->setScale(object->getScale() + scale);
					
					Vector3 translationAxis = (object->getPosition() - selection.getTranslationPart()).getNormalized();
					object->setPosition(object->getPosition() + (translationAxis * scale));
					LOG_INFO(object->getPosition());
					
					object->updateMatrix();
				}
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
				obj->updateFromMatrix();
				emit objectChanged(obj);
			}
		}
	}
	else
	{
		ImGuizmo::Enable(false);
	}
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

