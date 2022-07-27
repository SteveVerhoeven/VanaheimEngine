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

	protected:
		virtual void Initialize(Scene* pParentScene) override;
		virtual void PostInitialize(Scene* pParentScene) override;
		virtual void Update(const float elapsedSec) override;
		virtual void FixedUpdate(const float timeEachUpdate) override;

	private:
		ViewportUI* m_pViewportUI;

		void RotateCamera();
		void MoveCamera(KeyboardButton kButton);
};