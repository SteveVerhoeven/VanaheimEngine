#pragma once
#include "UI.h"
class CameraViewportUI : public UI
{
	public:
		CameraViewportUI();
		~CameraViewportUI();

		CameraViewportUI(const CameraViewportUI&) = delete;
		CameraViewportUI& operator=(const CameraViewportUI&) = delete;
		CameraViewportUI(CameraViewportUI&&) = delete;
		CameraViewportUI& operator=(CameraViewportUI&&) = delete;

	protected:
		void Initialize() override;
		void ShowWindow() override;

	private:
};