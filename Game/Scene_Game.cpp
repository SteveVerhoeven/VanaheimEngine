#include "GamePCH.h"
#include "Scene_Game.h"

#include <MoveCameraCommand.h>
#include <RotateCameraCommand.h>

#include <Material_ProcGen.h>
#include <Material_ProcGen_GPU.h>

Scene_Game::Scene_Game()
		   : Scene()
{}
Scene_Game::~Scene_Game()
{}

void Scene_Game::Initialize()
{
	GameObject* pGameObject{ new GameObject({}, {}, {}, "Terrian")};
	pGameObject->AddComponent(new TerrainGeneratorComponent());
	AddGameObject(pGameObject);
	//CreateLandscape_2DPlane();
	//CreateLandscape_VoxelsCPU();
	//CreateLandscape_VoxelsGPU();
	//CreateLandscape_2DPlaneOctree(true);
	//CreateLandscape_2DPlaneKDtree();

	// Inputs
	CreateInputs();

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

void Scene_Game::CreateInputs()
{
	InputManager* pInputManager{ Locator::GetInputManagerService() };

	pInputManager->AddKeyToMap(ControllerButton::ButtonUp, KeyboardButton::W, ButtonPressType::BUTTON_RELEASED, "MOVE_FORWARD", new MoveCameraCommand(true, false, false, false));
	pInputManager->AddKeyToMap(ControllerButton::ButtonDown, KeyboardButton::S, ButtonPressType::BUTTON_RELEASED, "MOVE_BACKWARD", new MoveCameraCommand(false, true, false, false));
	pInputManager->AddKeyToMap(ControllerButton::ButtonLeft, KeyboardButton::A, ButtonPressType::BUTTON_RELEASED, "MOVE_LEFT", new MoveCameraCommand(false, false, true, false));
	pInputManager->AddKeyToMap(ControllerButton::ButtonRight, KeyboardButton::D, ButtonPressType::BUTTON_RELEASED, "MOVE_RIGHT", new MoveCameraCommand(false, false, false, true));
	pInputManager->AddKeyToMap(ControllerButton::ButtonLThumbStick, KeyboardButton::E, ButtonPressType::BUTTON_HOLD, "ROTATE", new RotateCameraCommand());
}

//void Scene_Game::CreateLandscape_2DPlane()
//{
//	// Generate landscape	
//	GeneratorManager* pGeneratorManager{ Locator::GetGeneratorManagerService() };
//	TerrainGenerator* pProcGen(pGeneratorManager->GetGenerator<TerrainGenerator>());
//	Mesh* pMesh{ pProcGen->CreateNormalTerrain() };
//
//	// Model
//	ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };
//	Texture* pNormalTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/noiseMap.bmp") };
//	Texture* pColorTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/colorMap.bmp") };
//
//	UIManager* pUIManager{ Locator::GetUIManagerService() };
//	InspectorUI* pVanaheimUI{ pUIManager->GetUI<InspectorUI>() };
//	if (pVanaheimUI)
//	{
//		pVanaheimUI->AddObserver(pNormalTexture);
//		pVanaheimUI->AddObserver(pColorTexture);
//	}
//
//	Material_ProcGen* pMaterial = new Material_ProcGen();
//	pMaterial->AddTexture(pNormalTexture);
//	pMaterial->AddTexture(pColorTexture);
//
//	const std::string name{ "Landscape" };
//	const DirectX::XMFLOAT3 pos{ 0, -1, 0 };
//	Create3DObject(name, pos, pMesh, pMaterial);
//}
//void Scene_Game::CreateLandscape_VoxelsCPU()
//{
//	// Generate landscape	
//	GeneratorManager* pGeneratorManager{ Locator::GetGeneratorManagerService() };
//	TerrainGenerator* pProcGen(pGeneratorManager->GetGenerator<TerrainGenerator>());
//	Mesh* pMesh{ pProcGen->CreateVoxelTerrain_CPU() };
//
//	// Model
//	ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };
//	Texture* pNormalTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/noiseMap.bmp") };
//	Texture* pColorTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/colorMap.bmp") };
//
//	UIManager* pUIManager{ Locator::GetUIManagerService() };
//	InspectorUI* pVanaheimUI{ pUIManager->GetUI<InspectorUI>() };
//	if (pVanaheimUI)
//	{
//		pVanaheimUI->AddObserver(pNormalTexture);
//		pVanaheimUI->AddObserver(pColorTexture);
//	}
//
//	Material_ProcGen* pMaterial = new Material_ProcGen();
//	pMaterial->AddTexture(pNormalTexture);
//	pMaterial->AddTexture(pColorTexture);
//
//	const std::string name{ "Landscape" };
//	const DirectX::XMFLOAT3 pos{ 0, -1, 0 };
//	Create3DObject(name, pos, pMesh, pMaterial);
//}
//void Scene_Game::CreateLandscape_VoxelsGPU()
//{
//	// Generate landscape	
//	GeneratorManager* pGeneratorManager{ Locator::GetGeneratorManagerService() };
//	TerrainGenerator* pProcGen(pGeneratorManager->GetGenerator<TerrainGenerator>());
//	Mesh* pMesh{ pProcGen->CreateVoxelTerrain_GPU() };
//
//	// Model
//	ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };
//	Texture* pNormalTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/noiseMap.bmp") };
//	Texture* pColorTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/colorMap.bmp") };
//
//	UIManager* pUIManager{ Locator::GetUIManagerService() };
//	InspectorUI* pVanaheimUI{ pUIManager->GetUI<InspectorUI>() };
//	if (pVanaheimUI)
//	{
//		pVanaheimUI->AddObserver(pNormalTexture);
//		pVanaheimUI->AddObserver(pColorTexture);
//	}
//
//	Material_ProcGen_GPU* pMaterial = new Material_ProcGen_GPU();
//	pMaterial->AddTexture(pNormalTexture);
//	pMaterial->AddTexture(pColorTexture);
//
//	const std::string name{ "Landscape" };
//	const DirectX::XMFLOAT3 pos{ 0, -1, 0 };
//	Create3DObject(name, pos, pMesh, pMaterial);
//}
//void Scene_Game::CreateLandscape_2DPlaneOctree(const bool visualizeDataStructure)
//{
//	// Generate landscape	
//	GeneratorManager* pGeneratorManager{ Locator::GetGeneratorManagerService() };
//	TerrainGenerator* pProcGen(pGeneratorManager->GetGenerator<TerrainGenerator>());
//	Mesh* pMesh{ pProcGen->CreateNormalTerrain_OcTree(this, visualizeDataStructure) };
//
//	// Model
//	ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };
//	Texture* pNormalTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/noiseMap.bmp") };
//	Texture* pColorTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/colorMap.bmp") };
//
//	UIManager* pUIManager{ Locator::GetUIManagerService() };
//	InspectorUI* pVanaheimUI{ pUIManager->GetUI<InspectorUI>() };
//	if (pVanaheimUI)
//	{
//		pVanaheimUI->AddObserver(pNormalTexture);
//		pVanaheimUI->AddObserver(pColorTexture);
//	}
//
//	Material_ProcGen* pMaterial = new Material_ProcGen();
//	pMaterial->AddTexture(pNormalTexture);
//	pMaterial->AddTexture(pColorTexture);
//
//	const std::string name{ "Landscape" };
//	const DirectX::XMFLOAT3 pos{ 0, -1, 0 };
//	Create3DObject(name, pos, pMesh, pMaterial);
//}
//void Scene_Game::CreateLandscape_2DPlaneKDtree()
//{
//	// Generate landscape	
//	GeneratorManager* pGeneratorManager{ Locator::GetGeneratorManagerService() };
//	TerrainGenerator* pProcGen(pGeneratorManager->GetGenerator<TerrainGenerator>());
//	Mesh* pMesh{ pProcGen->CreateNormalTerrain_KDTree(this) };
//
//	// Model
//	ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };
//	Texture* pNormalTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/noiseMap.bmp") };
//	Texture* pColorTexture{ pResourceManager->LoadTexture("./Resources/Textures/Landscape/colorMap.bmp") };
//
//	UIManager* pUIManager{ Locator::GetUIManagerService() };
//	InspectorUI* pVanaheimUI{ pUIManager->GetUI<InspectorUI>() };
//	if (pVanaheimUI)
//	{
//		pVanaheimUI->AddObserver(pNormalTexture);
//		pVanaheimUI->AddObserver(pColorTexture);
//	}
//
//	Material_ProcGen* pMaterial = new Material_ProcGen();
//	pMaterial->AddTexture(pNormalTexture);
//	pMaterial->AddTexture(pColorTexture);
//
//	const std::string name{ "Landscape" };
//	const DirectX::XMFLOAT3 pos{ 0, -1, 0 };
//	Create3DObject(name, pos, pMesh, pMaterial);
//}