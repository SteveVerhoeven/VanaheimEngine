#include "VanirPCH.h"
#include "DemoUI.h"

DemoUI::DemoUI(const bool showDemo, const std::string& windowName)
	   : UI(windowName)
	   , m_ShowDemo(showDemo)
{}

void DemoUI::ShowWindow()
{
	if (!m_RenderUI)
		return;

	BeginWindowBase();
	ImGui::ShowDemoWindow(&m_ShowDemo);
	EndWindowBase();
}