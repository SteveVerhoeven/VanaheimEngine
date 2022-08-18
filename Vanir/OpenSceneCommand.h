#pragma once
#include <Command.h>

class OpenSceneCommand final : public Command
{
	public:
		OpenSceneCommand() = default;

		void Execute() override;

	private:
		void OpenScene();
};