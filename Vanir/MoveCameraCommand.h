#pragma once
#include "Command.h"

enum MoveDirection_Flags
{
	NO_MOVE = 0,
	FORWARD = 1 << 0,
	BACKWARD = 1 << 1,
	RIGHT = 1 << 2,
	LEFT = 1 << 3
};
DEFINE_ENUM_FLAG_OPERATORS(MoveDirection_Flags)

class GameObject;
class MoveCameraCommand final : public Command
{
	public:
		MoveCameraCommand(const MoveDirection_Flags& moveDirs);

		void Execute() override;

	private:
		MoveDirection_Flags m_MoveDirection;
		float m_MoveSpeed;
		GameObject* m_pCameraObject;

		void Move();
		void SetCameraObject();

		bool HasFlag(const MoveDirection_Flags& flag);
};