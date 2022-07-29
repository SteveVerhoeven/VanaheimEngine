#include "VanaheimPCH.h"
#include "SceneCameraMovement.h"

#include "Command.h"
#include "RotateCameraCommand.h"

SceneCameraMovement::SceneCameraMovement()
					: m_pViewportUI(nullptr)
{}

void SceneCameraMovement::Initialize(Scene* /*pParentScene*/)
{}
void SceneCameraMovement::PostInitialize(Scene* /*pParentScene*/)
{
	m_pViewportUI = Locator::GetUIManagerService()->GetUI<ViewportUI>();
}
void SceneCameraMovement::Update(const float /*elapsedSec*/)
{
	if (!m_pParentObject->GetComponent<CameraComponent>()->GetIsMainCamera())
		return;

	if (!m_pViewportUI->GetIsMouseInViewport())
		return;

	InputManager* pInputManager{ Locator::GetInputManagerService() };
	if (pInputManager->IsKeyDown(int(MouseButton::RMB)))
	{
		RotateCamera();

		if (pInputManager->IsKeyDown(int(KeyboardButton::W)))
			MoveCamera(KeyboardButton::W);
		else if (pInputManager->IsKeyDown(int(KeyboardButton::S)))
			MoveCamera(KeyboardButton::S);
		else if (pInputManager->IsKeyDown(int(KeyboardButton::A)))
			MoveCamera(KeyboardButton::A);
		else if (pInputManager->IsKeyDown(int(KeyboardButton::D)))
			MoveCamera(KeyboardButton::D);
	}
}
void SceneCameraMovement::FixedUpdate(const float /*timeEachUpdate*/)
{}

void SceneCameraMovement::RotateCamera()
{
	Command* pRot{ Locator::GetInputManagerService()->GetCommand(ControllerButton::ButtonLThumbStick) };
	pRot->Execute();
}
void SceneCameraMovement::MoveCamera(KeyboardButton kButton)
{
	Command* pRot{ Locator::GetInputManagerService()->GetCommand(kButton) };
	pRot->Execute();
}