#pragma once
#include "Command.h"

class GameObject;
class RotateCameraCommand final : public Command
{
	public:
		RotateCameraCommand();

		void Execute() override;

	private:
		float m_RotatingSpeed;
		GameObject* m_pCameraObject;

		void Rotate();
		void SetCameraObject();
};