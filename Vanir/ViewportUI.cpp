#include "VanirPCH.h"
#include "ViewportUI.h"

#include <Graphics.h>
#include <Window.h>
#include <Scene.h>
#include <SceneCameraMovement.h>

#include "OpenSceneCommand.h"

ViewportUI::ViewportUI()
		   : UI("Viewport")
		   , m_MouseInWindow(false)
{}
ViewportUI::~ViewportUI()
{}

void ViewportUI::Initialize(const Vanir& /*vEditor*/)
{}
void ViewportUI::Update()
{}
void ViewportUI::FixedUpdate()
{}
void ViewportUI::ShowWindow()
{
	if(!m_RenderUI)
		return;

	// Remove padding from the window
	PushStyle_RemovePadding();

	// Begin window with these flags
	ImGuiWindowFlags window_flags{};
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	BeginWindowBase(window_flags);

	// Get the window size
	ImGuiWindow* pWindow = ImGui::FindWindowByName(m_Name.c_str());
	const ImVec2 windowSize = pWindow->Size;

	// Calculate the image size;
	ImVec2 imageSize{};
	{
		float height{ windowSize.y };
		if (!pWindow->DockNode->IsHiddenTabBar())
			height -= ImGui::GetFrameHeight();

		imageSize = { height / 9 * 16, height };
	}

	// Set the position of where the image should start drawing
	if(windowSize.x > imageSize.x)
		ImGui::SetCursorPosX(windowSize.x / 2 - imageSize.x / 2);

	// Display the image
	Graphics* pGraphics{ Locator::GetGraphicsService() };
	ID3D11ShaderResourceView* pSRV{ pGraphics->GetShaderResourceView_Game() };
	ImGui::Image(pSRV, ImVec2{ imageSize.x, imageSize.y });

	// Drag Drop
	if(ImGui::BeginDragDropTarget())
	{
		if(const ImGuiPayload * payload{ ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM") })
		{
			const wchar_t* path{ (const wchar_t*)payload->Data };

			const std::filesystem::path relativeFilePath{ path };
			const std::filesystem::path preRelativeFilePath{ "../Game/Assets/" };
			const std::filesystem::path fullPath{ preRelativeFilePath / relativeFilePath };

			// Get the openscene command
			InputManager* pInputManager{ Locator::GetInputManagerService() };
			Command* pCommand{ pInputManager->GetCommand({ KeyboardButton::L_CTRL, KeyboardButton::O }) };
			OpenSceneCommand* pOpenSceneCommand{ static_cast<OpenSceneCommand*>(pCommand) };
			pOpenSceneCommand->SetPath(fullPath);
			pOpenSceneCommand->Execute();
		}	

		ImGui::EndDragDropTarget();
	}

	// Mouse check
	SceneCameraMovement* pSceneCameraMovement{ Locator::GetSceneManagerService()->GetActiveGameScene()->GetSceneCamera()->GetComponent<SceneCameraMovement>() };
	if (IsMouseInViewport(pWindow, windowSize))
		pSceneCameraMovement->SetMouseInViewport(true);
	else
		pSceneCameraMovement->SetMouseInViewport(false);

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
