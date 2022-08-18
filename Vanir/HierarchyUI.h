#pragma once
#include "UI.h"

class Scene;
class InspectorUI;
class HierarchyUI final : public UI
{
	public:
		HierarchyUI();
		~HierarchyUI();

		HierarchyUI(const HierarchyUI&) = delete;
		HierarchyUI& operator=(const HierarchyUI&) = delete;
		HierarchyUI(HierarchyUI&&) = delete;
		HierarchyUI& operator=(HierarchyUI&&) = delete;

		void Initialize(const Vanir& vEditor) override;
		void Update() override;
		void FixedUpdate() override;
		void ShowWindow() override;
		
		void SetActiveScene(Scene* pScene) { m_pScene = pScene; }

	protected:

	private:
		Scene* m_pScene;
		size_t m_SelectedGameObject;
		InspectorUI* m_pInspectorUI;

		void Draw();
};