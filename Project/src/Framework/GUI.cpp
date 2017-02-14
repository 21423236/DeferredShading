#include <Framework/GUI.h>
#include <Framework/Window.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>

GUI::GUI()
{
}


GUI::~GUI()
{
}

void GUI::Initialize(Window const & window)
{
	ImGui_Impl_Init(window.GetHandle());
}

void GUI::NewFrame(int const & width, int const & height)
{
	ImGui_Impl_NewFrame(width, height);
}

void GUI::GenerateGUI(Scene const & scene)
{
	ImGui::Begin("Scene");
	ImGui::Text("Will contain information about the scene");
	ImGui::End();
}

void GUI::EndFrame()
{
	ImGui::Render();
}

void GUI::Finalize()
{
	ImGui_Impl_Shutdown();
}