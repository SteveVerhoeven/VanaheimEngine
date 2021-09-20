#pragma once
#include "Scene.h"

class Scene_GPUInstance final : public Scene
{
	public:
		Scene_GPUInstance();
		virtual ~Scene_GPUInstance() override;

		Scene_GPUInstance(const Scene_GPUInstance& other) = delete;
		Scene_GPUInstance(Scene_GPUInstance&& other) = delete;
		Scene_GPUInstance& operator=(const Scene_GPUInstance& other) = delete;
		Scene_GPUInstance& operator=(Scene_GPUInstance&& other) = delete;

		virtual void Initialize() override;
		virtual void PostInitialize() override;
		virtual void Update(const float elapsedSec) override;
		virtual void FixedUpdate(const float timeEachUpdate) override;
		virtual void LateUpdate() override;
		virtual void Render() const override;

	private:
		void CreateInputs();
};