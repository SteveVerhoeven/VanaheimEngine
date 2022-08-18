#pragma once
#include <Command.h>


class SaveSceneCommand final : public Command
{
	public:
		SaveSceneCommand() = default;

		void Execute() override;

	private:
		void SaveScene();
};