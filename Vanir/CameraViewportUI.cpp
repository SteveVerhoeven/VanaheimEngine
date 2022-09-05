#include "VanirPCH.h"
#include "CameraViewportUI.h"

#include "Graphics.h"
#include "Window.h"

CameraViewportUI::CameraViewportUI()
				 : UI("CameraViewport")
{}
CameraViewportUI::~CameraViewportUI()
{}

void CameraViewportUI::Initialize(const Vanir& /*vEditor*/)
{}
void CameraViewportUI::Update()
{
}
void CameraViewportUI::FixedUpdate()
{
}
void CameraViewportUI::ShowWindow()
{
	if (!m_RenderUI)
		return;

	PushStyle_RemovePadding();

	ImGuiWindowFlags window_flags{};
	BeginWindowBase(window_flags);

	Graphics* pGraphics{ Locator::GetGraphicsService() };
	ID3D11ShaderResourceView* pSRV{ pGraphics->GetShaderResourceView_Camera() };
	const float width{ ImGui::GetWindowWidth() };
	const float height{ ImGui::GetWindowHeight() };
	ImGui::Image(pSRV, ImVec2{ width, height });

	EndWindowBase();

	ImGui::PopStyleVar();
}