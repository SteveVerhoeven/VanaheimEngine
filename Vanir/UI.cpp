#include "VanirPCH.h"
#include "UI.h"

UI::UI(const std::string& windowName)
   : m_RenderUI(true)
   , m_Name(windowName)
{}

void UI::BeginWindowBase()
{
	ImGui::Begin(m_Name.c_str(), &m_RenderUI);
}
void UI::BeginWindowBase(const ImGuiWindowFlags& windowFlags)
{
	ImGui::Begin(m_Name.c_str(), &m_RenderUI, windowFlags);
}
void UI::EndWindowBase()
{
	ImGui::End();
}

void UI::PushStyle_RemovePadding()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
}
void UI::PopStyleVar()
{
	ImGui::PopStyleVar();
}