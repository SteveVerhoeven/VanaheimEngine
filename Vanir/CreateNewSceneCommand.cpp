#include "VanirPCH.h"
#include "CreateNewSceneCommand.h"

void CreateNewSceneCommand::Execute()
{
	CreateNewScene();
}
void CreateNewSceneCommand::CreateNewScene()
{
	Locator::GetSceneManagerService()->ReplaceCurrentGameSceneByNewOne();
}