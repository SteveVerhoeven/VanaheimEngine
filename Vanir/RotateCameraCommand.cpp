#include "VanirPCH.h"
#include "RotateCameraCommand.h"

#include "Scene.h"
#include "Timer.h"
#include "Mouse.h"

RotateCameraCommand::RotateCameraCommand()
					: m_RotatingSpeed(20.f)
					, m_pCameraObject(nullptr)
{}

void RotateCameraCommand::Execute()
{
	if (m_pCameraObject == nullptr)
		SetCameraObject();

	Rotate();
}

void RotateCameraCommand::Rotate()
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	const float elapsedSec{ Locator::GetTimerService()->GetElapsedTime() };
	
	const POINT mouseMovementPoint{ Locator::GetInputManagerService()->GetMouse()->GetMouseMovement() };
	const DirectX::XMFLOAT2 mouseMovement{ (float)mouseMovementPoint.x, (float)mouseMovementPoint.y };

	TransformComponent* pTransformComponent{ m_pCameraObject->GetComponent<TransformComponent>() };
	DirectX::XMFLOAT4 currentRotation{ pTransformComponent->GetRotation(false) };
	DirectX::XMFLOAT3 newRotation{};
	newRotation.x = currentRotation.x + (mouseMovement.y * m_RotatingSpeed * elapsedSec);
	newRotation.y = currentRotation.y + (mouseMovement.x * m_RotatingSpeed * elapsedSec);
	newRotation.z = currentRotation.z;

	pTransformComponent->Rotate(newRotation);

	Update_Flags flags{};
	flags |= Update_Flags::VIEW;
	flags |= Update_Flags::PROJECTION;
	flags |= Update_Flags::VIEWINVERSE;
	flags |= Update_Flags::VIEWPROJECTION;
	flags |= Update_Flags::VIEWPROJECTIONINVERSE;
	m_pCameraObject->GetComponent<CameraComponent>()->SetUpdateFlags(flags);
}

void RotateCameraCommand::SetCameraObject()
{
	SceneManager* pSceneManager{ Locator::GetSceneManagerService() };
	Scene* pScene{ pSceneManager->GetActiveGameScene() };
	m_pCameraObject = pScene->GetSceneCamera();
}