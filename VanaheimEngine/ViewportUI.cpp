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

	// Remove padding from the window
	PushStyle_RemovePadding();

	// Begin window with these flags
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoDecoration;
	BeginWindowBase(window_flags);
	
	// Image
	ImGuiWindow* pWindow = ImGui::FindWindowByName(m_Name.c_str());
	const ImVec2 windowSize = pWindow->Size;

	Graphics* pGraphics{ Locator::GetGraphicsService() };
	ID3D11ShaderResourceView* pSRV{ pGraphics->GetShaderResourceView_Game() };
	
	const ImVec2 imageSize = { 1280, 720 };
	if (windowSize.x > imageSize.x)
		ImGui::SetCursorPosX(windowSize.x / 2 - 1280 / 2);
	
	ImGui::Image(pSRV, ImVec2{ imageSize.x, imageSize.y });
	
	// Mouse check
	if (IsMouseInViewport(pWindow, windowSize))
		m_MouseInWindow = true;
	else
		m_MouseInWindow = false;

	// Close window
	EndWindowBase();

	// Remove padding style
	ImGui::PopStyleVar();
}

bool ViewportUI::IsMouseInViewport(ImGuiWindow* pWindow, const ImVec2& windowSizeMax) const
{
	// Mouse position
	ImVec2 mousePos = ImGui::GetMousePos();

	// Viewport window size
	ImVec2 windowSizeMin = pWindow->DC.CursorStartPos;

	// check inside or out
	if (windowSizeMin.x < mousePos.x && mousePos.x < windowSizeMin.x + windowSizeMax.x &&
		windowSizeMin.y < mousePos.y && mousePos.y < windowSizeMin.y + windowSizeMax.y)
		return true;

	return false;
}
