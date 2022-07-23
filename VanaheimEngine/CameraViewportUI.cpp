#include "VanaheimPCH.h"
#include "CameraViewportUI.h"

#include "Graphics.h"
#include "Window.h"

CameraViewportUI::CameraViewportUI()
		   : UI("CameraViewport", DirectX::XMFLOAT2{ 0.f, 0.f }, DirectX::XMFLOAT2{ 0.f, 0.f })
{}
CameraViewportUI::~CameraViewportUI()
{}

void CameraViewportUI::Initialize()
{}
void CameraViewportUI::ShowWindow()
{
	if (!m_RenderUI)
		return;

	BeginWindowBase();

	Graphics* pGraphics{ Locator::GetGraphicsService() };
	const float width{ ImGui::GetWindowWidth() };
	const float height{ ImGui::GetWindowHeight() };
	ID3D11ShaderResourceView* pSRV{ pGraphics->GetShaderResourceView_Camera() };
	ImGui::Image(pSRV, ImVec2{ width, height });

	EndWindowBase();
}