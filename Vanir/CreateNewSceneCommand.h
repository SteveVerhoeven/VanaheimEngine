#pragma once
#include <Command.h>

class CreateNewSceneCommand final : public Command
{
	public:
		CreateNewSceneCommand() = default;

		void Execute() override;

	private:
		void CreateNewScene();
};