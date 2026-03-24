#include "GamePCH.h"
#include "Scene_ObjectLoading.h"

#include "Material_Basic.h"

Scene_ObjectLoading::Scene_ObjectLoading()
		   : Scene()
{}
Scene_ObjectLoading::~Scene_ObjectLoading()
{}

void Scene_ObjectLoading::Initialize()
{
	GameObject* pGameObject{ new GameObject("Object") };
	ModelComponent* pModelComponent{ new ModelComponent("../VanaheimEngine/Resources/Meshes/Crap.obj")};
	pModelComponent->AddMaterial(new Material_Basic());
	pGameObject->AddComponent(pModelComponent);
	AddGameObject(pGameObject);

	ActivateScene();
	Scene::Initialize();
}
void Scene_ObjectLoading::PostInitialize()
{ Scene::PostInitialize(); }
void Scene_ObjectLoading::Update(const float elapsedSec)
{ Scene::Update(elapsedSec); }
void Scene_ObjectLoading::FixedUpdate(const float timeEachUpdate)
{ Scene::FixedUpdate(timeEachUpdate); }
void Scene_ObjectLoading::LateUpdate()
{ Scene::LateUpdate(); }
void Scene_ObjectLoading::Render() const
{ Scene::Render(); }