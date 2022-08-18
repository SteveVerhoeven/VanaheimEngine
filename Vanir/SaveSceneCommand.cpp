#include "VanirPCH.h"
#include "SaveSceneCommand.h"

#include <SceneSerializer.h>

void SaveSceneCommand::Execute()
{
	SaveScene();
}
void SaveSceneCommand::SaveScene()
{
	const std::string filePath{ WindowsUtils::FileDialogs::SaveFile("Vanaheim Scene (*.Vanaheim)\0*.Vanaheim\0") };

	if (!filePath.empty())
	{
		SceneSerializer serializer{};
		serializer.Serialize(filePath, Locator::GetSceneManagerService()->GetActiveGameScene());
	}
}