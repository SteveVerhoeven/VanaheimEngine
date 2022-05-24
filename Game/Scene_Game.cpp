#include "GamePCH.h"
#include "Scene_Game.h"

#include "Material_GPUInstance.h"

Scene_Game::Scene_Game()
		   : Scene()
{}
Scene_Game::~Scene_Game()
{}

void Scene_Game::Initialize()
{
	// Bunny 1
	const std::string name1{ "Bunny 1" };
	const DirectX::XMFLOAT3 pos1{ 10, 0, 0 };
	Material_GPUInstance* pMaterial1 = new Material_GPUInstance();
	Create3DObject(name1, pos1, "./Resources/Meshes/lowpoly_bunny.obj", pMaterial1);

	// Bunny 2
	const std::string name2{ "Bunny 1" };
	const DirectX::XMFLOAT3 pos2{ -10, 0, 0 };
	Material_GPUInstance* pMaterial2 = new Material_GPUInstance();
	Create3DObject(name2, pos2, "./Resources/Meshes/lowpoly_bunny.obj", pMaterial2);

	// Inputs
	CreateBaseInputs();

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