#include "GamePCH.h"
#include "Scene_Game.h"

Scene_Game::Scene_Game()
		   : Scene()
{}
Scene_Game::~Scene_Game()
{}

void Scene_Game::Initialize()
{
	GameObject* pGameObject{ new GameObject({}, {}, {1, 1, 1}, "Terrain") };
	pGameObject->AddComponent(new TerrainGeneratorComponent());
	AddGameObject(pGameObject);

	ActivateScene();
	Scene::Initialize();
}
void Scene_Game::PostInitialize()
{ Scene::PostInitialize(); }
void Scene_Game::Update(const float elapsedSec)
{ Scene::Update(elapsedSec); }
void Scene_Game::FixedUpdate(const float timeEachUpdate)
{ Scene::FixedUpdate(timeEachUpdate); }
void Scene_Game::LateUpdate()
{ Scene::LateUpdate(); }
void Scene_Game::Render() const
{ Scene::Render(); }