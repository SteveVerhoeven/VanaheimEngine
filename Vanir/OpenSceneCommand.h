#pragma once
#include <Command.h>

class OpenSceneCommand final : public Command
{
	public:
		OpenSceneCommand();

		void Execute() override;

		void SetPath(const std::filesystem::path& scenePath) { m_Path = scenePath; }

	private:
		std::filesystem::path m_Path;

		void OpenScene();
};