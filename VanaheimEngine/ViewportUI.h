#pragma once
#include "UI.h"

class ViewportUI final : public UI
{
	public:
		ViewportUI();
		~ViewportUI();

		ViewportUI(const ViewportUI&) = delete;
		ViewportUI& operator=(const ViewportUI&) = delete;
		ViewportUI(ViewportUI&&) = delete;
		ViewportUI& operator=(ViewportUI&&) = delete;

		bool GetIsMouseInViewport() const { return m_MouseInWindow; }

	protected:
		void Initialize() override;
		void ShowWindow() override;

	private:
		bool m_MouseInWindow;
		bool IsMouseInViewport(ImGuiWindow* pWindow, const ImVec2& windowSize) const;
};