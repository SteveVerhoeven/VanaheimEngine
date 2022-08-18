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

		void Initialize(const Vanir& vEditor) override;
		void Update() override;
		void FixedUpdate() override;
		void ShowWindow() override;
		
		bool GetIsMouseInViewport() const { return m_MouseInWindow; }

	protected:

	private:
		bool m_MouseInWindow;
		bool IsMouseInViewport(ImGuiWindow* pWindow, const ImVec2& windowSize) const;
};