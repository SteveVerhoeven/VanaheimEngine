#pragma once
#include "UI.h"

class CreateNewSceneCommand;
class SaveSceneCommand;
class OpenSceneCommand;
class MenubarUI final : public UI
{
	public:
		MenubarUI();
		~MenubarUI();

		MenubarUI(const MenubarUI&) = delete;
		MenubarUI& operator=(const MenubarUI&) = delete;
		MenubarUI(MenubarUI&&) = delete;
		MenubarUI& operator=(MenubarUI&&) = delete;

		void Initialize(const Vanir& vEditor) override;
		void Update() override;
		void FixedUpdate() override;
		void ShowWindow() override;

		void Render(const bool useCustomTitleBar);

		bool BeginMenuBar(const ImRect& barRectangle);
		void MenuButtons();
		void EndMenuBar();

		void SetCNSC(CreateNewSceneCommand* pCreateNewSceneCommand) { m_pCreateNewSceneCommand = pCreateNewSceneCommand; }

	protected:

	private:
		CreateNewSceneCommand* m_pCreateNewSceneCommand;
		SaveSceneCommand* m_pSaveSceneCommand;
		OpenSceneCommand* m_pOpenSceneCommand;

		void FileMenu();
		void EditMenu();
		void WindowMenu();
		void ToolMenu();
};