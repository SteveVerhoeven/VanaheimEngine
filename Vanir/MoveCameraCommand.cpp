#include "VanirPCH.h"
#include "MoveCameraCommand.h"

#include "Scene.h"
#include "Timer.h"

MoveCameraCommand::MoveCameraCommand(const MoveDirection_Flags & moveDirs)
				  : m_MoveDirection(moveDirs)
				  , m_MoveSpeed(2500.f) //2500
				  , m_pCameraObject(nullptr)
{}

void MoveCameraCommand::Execute()
{
	if (m_pCameraObject == nullptr)
		SetCameraObject();

	Move();
}

void MoveCameraCommand::Move()
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	TransformComponent* pTransformComponent{ m_pCameraObject->GetComponent<TransformComponent>() };
	const DirectX::XMFLOAT3 currentPosition{ pTransformComponent->GetWorldPosition() };
	const DirectX::XMVECTOR currentPositionVector{ DirectX::XMLoadFloat3(&currentPosition) };

	const float elapsedSec{ Locator::GetTimerService()->GetElapsedTime() };

	bool hasMoved{ false };
	// ****************************
	// Move LEFT / RIGHT
	// ****************************
	if (HasFlag(MoveDirection_Flags::LEFT) || HasFlag(MoveDirection_Flags::RIGHT))
	{
		const DirectX::XMFLOAT3 right = pTransformComponent->GetRight();
		const DirectX::XMVECTOR rightVector{ DirectX::XMLoadFloat3(&right) };

		DirectX::XMVECTOR newPosition{};
		if (HasFlag(MoveDirection_Flags::LEFT))
			newPosition = currentPositionVector - (rightVector * m_MoveSpeed * elapsedSec);
		else if (HasFlag(MoveDirection_Flags::RIGHT))
			newPosition = currentPositionVector + (rightVector * m_MoveSpeed * elapsedSec);

		pTransformComponent->Translate(newPosition);

		hasMoved = true;		
	}

	// ****************************
	//  Move FORWARD / BACKWARD
	// ****************************
	if (HasFlag(MoveDirection_Flags::FORWARD) || HasFlag(MoveDirection_Flags::BACKWARD))
	{
		const DirectX::XMFLOAT3 forward = pTransformComponent->GetForward();
		const DirectX::XMVECTOR forwardVector{ DirectX::XMLoadFloat3(&forward) };
	
		DirectX::XMVECTOR newPosition{};
		if (HasFlag(MoveDirection_Flags::FORWARD))
			newPosition = currentPositionVector + (forwardVector * m_MoveSpeed * elapsedSec);
		else if (HasFlag(MoveDirection_Flags::BACKWARD))
			newPosition = currentPositionVector - (forwardVector * m_MoveSpeed * elapsedSec);
	
		pTransformComponent->Translate(newPosition);

		hasMoved = true;
	}	

	if (hasMoved)
	{
		Update_Flags flags{};
		flags |= Update_Flags::VIEW;
		flags |= Update_Flags::PROJECTION;
		flags |= Update_Flags::VIEWINVERSE;
		flags |= Update_Flags::VIEWPROJECTION;
		flags |= Update_Flags::VIEWPROJECTIONINVERSE;
		m_pCameraObject->GetComponent<CameraComponent>()->SetUpdateFlags(flags);
	}
}

void MoveCameraCommand::SetCameraObject()
{
	SceneManager* pSceneManager{ Locator::GetSceneManagerService() };
	Scene* pScene{ pSceneManager->GetActiveGameScene() };
	m_pCameraObject = pScene->GetSceneCamera();
}

bool MoveCameraCommand::HasFlag(const MoveDirection_Flags& flag)
{ return (m_MoveDirection & flag); }