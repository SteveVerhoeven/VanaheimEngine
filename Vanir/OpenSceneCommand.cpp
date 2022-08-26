#include "VanirPCH.h"
#include "OpenSceneCommand.h"

#include <SceneSerializer.h>
#include "InspectorUI.h"
#include "Vanir.h"

OpenSceneCommand::OpenSceneCommand()
				 : m_Path("")
{}

void OpenSceneCommand::Execute()
{
	OpenScene();
}
void OpenSceneCommand::OpenScene()
{
	if(m_Path.extension() != ".Vanaheim")
		return;

	const std::string filePath{ (m_Path != "") ? m_Path.string() : WindowsUtils::FileDialogs::OpenFile("Vanaheim Scene (*.Vanaheim)\0*.Vanaheim\0")};

	if (!filePath.empty())
	{
		Scene* pScene{ Locator::GetSceneManagerService()->ReplaceCurrentGameSceneByNewOne() };
		InspectorUI* pInspectorUI{ Locator::GetEditorService()->GetUI<InspectorUI>() };
		SceneSerializer serializer{};
		serializer.Deserialize(filePath, pScene, pInspectorUI);
	}
}