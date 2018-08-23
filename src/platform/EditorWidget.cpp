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
	
	io.MouseClicked[0] = mouse.isKeyDown(MOUSE_BUTTON_LEFT);
	io.MouseClicked[1] = mouse.isKeyDown(MOUSE_BUTTON_RIGHT);
	io.MouseClicked[2] = mouse.isKeyDown(MOUSE_BUTTON_MIDDLE);
}

void EditorWidget::paintGL()
{
	LevelWidget::paintGL();
	
	updateImGuiInput();
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
		
	ImGuizmo::BeginFrame();
	
	Matrix4x4 id;
	id.loadIdentity();
	id.setRotationX(90);
	
	// ImGuizmo::DrawGrid(getCamera().getViewMatrix().entries, getCamera().getProjectionMatrix().entries, id.entries, 10.f);	
	ImGuizmo::Manipulate(getCamera().getViewMatrix().entries, 
				getCamera().getProjectionMatrix().entries, 
				ImGuizmo::OPERATION::TRANSLATE, 
				ImGuizmo::MODE::WORLD, 
				id, NULL, 
				0, 
				NULL, 
				NULL);
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
