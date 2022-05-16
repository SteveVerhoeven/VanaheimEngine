#pragma once
#include <Scene.h>

class Scene_Game final : public Scene
{
	public:
		Scene_Game();
		virtual ~Scene_Game() override;

		Scene_Game(const Scene_Game& other) = delete;
		Scene_Game(Scene_Game&& other) = delete;
		Scene_Game& operator=(const Scene_Game& other) = delete;
		Scene_Game& operator=(Scene_Game&& other) = delete;

		virtual void Initialize() override;
		virtual void PostInitialize() override;
		virtual void Update(const float elapsedSec) override;
		virtual void FixedUpdate(const float timeEachUpdate) override;
		virtual void LateUpdate() override;
		virtual void Render() const override;

	private:
		
};