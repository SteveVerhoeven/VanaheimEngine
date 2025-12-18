#pragma once
#include "UI.h"

class Texture;
class MenubarUI;
class TitlebarUI final : public UI
{
	public:
		TitlebarUI();
		~TitlebarUI();

		TitlebarUI(const TitlebarUI&) = delete;
		TitlebarUI& operator=(const TitlebarUI&) = delete;
		TitlebarUI(TitlebarUI&&) = delete;
		TitlebarUI& operator=(TitlebarUI&&) = delete;

		void Initialize(const Vanir& vEditor) override;
		void Update() override;
		void FixedUpdate() override;
		void ShowWindow() override;

		void Render(const bool drawCustomTitleBar, float& outTitlebarHeight, MenubarUI* pMenuBarUI);

	protected:

	private:
		bool m_TitleBarHovered;
		// Icons
		/* Logo */
		Texture* m_pLogoIcon;
		/* Minimize */
		Texture* m_pMinIcon;
		/* Maximize */
		Texture* m_pMaxIcon;
		/* Close */
		Texture* m_pCloseIcon;

		void Logo(const ImVec2& windowPadding, const float& titlebarVerticalOffset);
		void MenuBar(const bool drawCustomTitleBar, const float& titlebarVerticalOffset, const ImVec2& windowPadding, MenubarUI* pMenuBarUI);
		void Title(const ImVec2& windowPadding);
		void MinimizeButton(const ImVec2& size, const ImVec2& posOffset, const ImU32& buttonColorNormal) const;
		void MaximizeButton(const ImVec2& size, const ImVec2& posOffset, const ImU32& buttonColorNormal, const bool isMaximized) const;
		void CloseButton(const ImVec2& size, const ImVec2& posOffset, const ImU32& buttonColorNormal) const;
};

