#include "VanaheimPCH.h"
#include "ViewportUI.h"

#include "Graphics.h"
#include "Window.h"

ViewportUI::ViewportUI()
		   : UI("Viewport", DirectX::XMFLOAT2{ 0.f, 0.f }, DirectX::XMFLOAT2{ 0.f, 0.f })
{}
ViewportUI::~ViewportUI()
{}

void ViewportUI::Initialize()
{}
void ViewportUI::ShowWindow()
{
	if (!m_RenderUI)
		return;

	PushStyle_RemovePadding();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoDecoration;

	BeginWindowBase(window_flags);
	
	ImGuiWindow* window = ImGui::FindWindowByName(m_Name.c_str());
	UNREFERENCED_PARAMETER(window);
	auto s = window->ContentSize;
	auto s1 = window->ContentSizeExplicit;
	auto s2 = window->ContentSizeIdeal;
	auto s3 = window->Size;
	auto s4 = window->SizeFull;
	auto s5 = window->WindowBorderSize;
	UNREFERENCED_PARAMETER(s);
	UNREFERENCED_PARAMETER(s1);
	UNREFERENCED_PARAMETER(s2);
	UNREFERENCED_PARAMETER(s3);
	UNREFERENCED_PARAMETER(s4);
	UNREFERENCED_PARAMETER(s5);

	Graphics* pGraphics{ Locator::GetGraphicsService() };
	ID3D11ShaderResourceView* pSRV{ pGraphics->GetShaderResourceView_Game() };
	const ImVec2 imageSize = { 1280, 720 };
	if (s3.x > imageSize.x)
		ImGui::SetCursorPosX(1920 / 2 - 1280 / 2);
	ImGui::Image(pSRV, ImVec2{ imageSize.x, imageSize.y });
	EndWindowBase();

	ImGui::PopStyleVar();
}