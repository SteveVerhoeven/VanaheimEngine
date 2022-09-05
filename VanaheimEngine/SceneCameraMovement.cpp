#include "VanaheimPCH.h"
#include "SceneCameraMovement.h"

#include "Command.h"

SceneCameraMovement::SceneCameraMovement()
					: m_MouseIsInViewport(nullptr)
{}

void SceneCameraMovement::Initialize(Scene* /*pParentScene*/)
{}
void SceneCameraMovement::PostInitialize(Scene* /*pParentScene*/)
{}
void SceneCameraMovement::Update(const float /*elapsedSec*/)
{
	if (!m_pParentObject->GetComponent<CameraComponent>()->GetIsMainCamera())
		return;

	if (!m_MouseIsInViewport)
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