#pragma once
#include "Component.h"
class SceneCameraMovement : public Component
{
	public:
		SceneCameraMovement();
		virtual ~SceneCameraMovement() = default;

		SceneCameraMovement(const SceneCameraMovement&) = delete;
		SceneCameraMovement(SceneCameraMovement&&) noexcept = delete;
		SceneCameraMovement& operator=(const SceneCameraMovement&) = delete;
		SceneCameraMovement& operator=(SceneCameraMovement&&) noexcept = delete;

		void SetMouseInViewport(const bool mouseInViewport) { m_MouseIsInViewport = mouseInViewport; }

	protected:
		virtual void Initialize(Scene* pParentScene) override;
		virtual void PostInitialize(Scene* pParentScene) override;
		virtual void Update(const float elapsedSec) override;
		virtual void FixedUpdate(const float timeEachUpdate) override;

	private:
		bool m_MouseIsInViewport;

		void RotateCamera();
		void MoveCamera(KeyboardButton kButton);
};