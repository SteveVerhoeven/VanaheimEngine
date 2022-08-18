#pragma once
#include "UI.h"

class CameraViewportUI final : public UI
{
	public:
		CameraViewportUI();
		~CameraViewportUI();

		CameraViewportUI(const CameraViewportUI&) = delete;
		CameraViewportUI& operator=(const CameraViewportUI&) = delete;
		CameraViewportUI(CameraViewportUI&&) = delete;
		CameraViewportUI& operator=(CameraViewportUI&&) = delete;

		void Initialize(const Vanir& vEditor) override;
		void Update() override;
		void FixedUpdate() override;
		void ShowWindow() override;

	protected:	

	private:
};