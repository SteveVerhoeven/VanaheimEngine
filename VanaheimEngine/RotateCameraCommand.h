#pragma once
#include "Command.h"

class GameObject;
class RotateCameraCommand final : public Command
{
	public:
		RotateCameraCommand();

		void Execute() override;
		void SetMouseMovement(const DirectX::XMFLOAT2& mouseMovement) { m_MouseMovement = mouseMovement; }

	private:
		float m_TotalYaw;
		float m_TotalPitch;
		float m_RotatingSpeed;
		DirectX::XMFLOAT2 m_MouseMovement;
		GameObject* m_pCameraObject;


		void Rotate();
		void SetCameraObject();
};