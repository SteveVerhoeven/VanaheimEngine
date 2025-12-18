#include "VanirPCH.h"
#include "CreateNewSceneCommand.h"

CreateNewSceneCommand::CreateNewSceneCommand(HierarchyUI* pHierarchyUI, InspectorUI* pInspectorUI)
					  : m_pHierarchyUI(pHierarchyUI)
					  , m_pInspectorUI(pInspectorUI)
{}

void CreateNewSceneCommand::Execute()
{
	CreateNewScene();
}
void CreateNewSceneCommand::CreateNewScene()
{
	Scene* pScene = Locator::GetSceneManagerService()->ReplaceCurrentGameSceneByNewOne();

	// Set the new scene so that the hierarchy can draw the objects
	m_pHierarchyUI->SetActiveScene(pScene);

	// Remove them from the observer list
	m_pInspectorUI->RemoveObservers();
}