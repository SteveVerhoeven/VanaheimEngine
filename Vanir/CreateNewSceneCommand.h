#pragma once
#include <Command.h>

class HierarchyUI;
class InspectorUI;
class CreateNewSceneCommand final : public Command
{
	public:
		CreateNewSceneCommand(HierarchyUI* pHierarchyUI, InspectorUI* pInspectorUI);

		void Execute() override;

		Scene* GetScene() { return m_pScene; }

	private:
		HierarchyUI* m_pHierarchyUI;
		InspectorUI* m_pInspectorUI;

		Scene* m_pScene;

		void CreateNewScene();
};