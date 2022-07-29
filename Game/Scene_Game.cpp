#include "GamePCH.h"
#include "Scene_Game.h"

Scene_Game::Scene_Game()
		   : Scene()
{}
Scene_Game::~Scene_Game()
{}

void Scene_Game::Initialize()
{
	// Terrain generator
	GameObject* pGameObjectTerrain{ new GameObject("Terrain")};
	pGameObjectTerrain->AddComponent(new TerrainGeneratorComponent());
	AddGameObject(pGameObjectTerrain);

	TransformComponent* pTransformComponentTerrain{ pGameObjectTerrain->GetComponent<TransformComponent>() };
	pTransformComponentTerrain->Translate(-36.f, 10.f, 0.f);

	// Side Camera
	GameObject* pGameObjectCamera{ new GameObject("Side camera") };
	pGameObjectCamera->AddComponent(new CameraComponent());
	AddGameObject(pGameObjectCamera);

	TransformComponent* pTransformComponentCamera{ pGameObjectCamera->GetComponent<TransformComponent>() };
	pTransformComponentCamera->Translate(35.7f, 14.3f, -77.6f);
	pTransformComponentCamera->Rotate(0.f, -49.3f, 0.f);

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