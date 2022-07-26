#include "VanaheimPCH.h"
#include "Scene.h"

#include "Material.h"
#include "Material_GPUInstance.h"

#include "Line.h"

#include "MoveCameraCommand.h"
#include "RotateCameraCommand.h"

#include "SceneCameraMovement.h"

Scene::Scene()
	  : m_Cleanup(false)
	  , m_IsActive(false)
	  , m_Name("")
	  , m_pSceneCameraGO(nullptr)
	  , m_pGameObjects(std::vector<GameObject*>())
{}
Scene::~Scene()
{
	DELETE_POINTERS(m_pGameObjects, m_pGameObjects.size());
	DELETE_POINTER(m_pSceneCameraGO);
}

void Scene::Initialize()
{
	// Scene camera
	const std::string name{ "Camera-Main" };
	const DirectX::XMFLOAT3 pos{ 0, 50, -200 };
	CreateSceneCamera(name, pos);

	// Initialize objects
	for (GameObject* pObject : m_pGameObjects)
		pObject->Initialize();

	// Inputs
	CreateSceneCameraInputs();
}
void Scene::PostInitialize()
{
	m_pSceneCameraGO->PostInitialize();

	for (GameObject* pObject : m_pGameObjects)
		pObject->PostInitialize();
}
void Scene::Update(const float elapsedSec)
{
	if (!m_IsActive)
		return;

	m_pSceneCameraGO->Update(elapsedSec);

	for (GameObject* pObject : m_pGameObjects)
		pObject->Update(elapsedSec);

	Locator::GetResourceManagerService()->ResetInstancedMeshes();
}
void Scene::FixedUpdate(const float timeEachUpdate)
{
	if (!m_IsActive)
		return;

	m_pSceneCameraGO->FixedUpdate(timeEachUpdate);

	for (GameObject* pObject : m_pGameObjects)
		pObject->FixedUpdate(timeEachUpdate);
}
void Scene::LateUpdate()
{
	if (!m_IsActive)
		return;

	CleanScene();
}
void Scene::Render() const
{
	if (!m_IsActive)
		return;

	for (GameObject* pObject : m_pGameObjects)
		pObject->Render();
}

void Scene::AddEmptyGameObject()
{
	GameObject* pGameObject{ new GameObject("Empty Game Object")};
	AddGameObject(pGameObject);
}
void Scene::AddCamera()
{
	GameObject* pGameObject{ new GameObject("Camera")};
	pGameObject->AddComponent(new CameraComponent());
	AddGameObject(pGameObject);
}
void Scene::AddGameObject(GameObject* pGameObject)
{
	pGameObject->SetParentScene(this);

	// Find duplicates
	auto comp = pGameObject->GetComponent<LineComponent>();
	if (comp)
	{
		auto mesh = comp->GetMesh();
		if (mesh)
		{
			if (!Locator::GetResourceManagerService()->Load3DMesh(dynamic_cast<Mesh_Base*>(mesh), pGameObject))
			{
				DELETE_POINTER(pGameObject);
				return;
			}
		}
	}

	m_pGameObjects.push_back(pGameObject);
}
void Scene::RemoveGameObject(GameObject* pObject)
{
	auto object = std::find(m_pGameObjects.begin(), m_pGameObjects.end(), pObject);
	if (object != m_pGameObjects.end())
	{
		auto distance{ std::distance(m_pGameObjects.begin(), object) };
		m_pGameObjects.erase(m_pGameObjects.begin() + distance);
	}
	
	DELETE_POINTER(pObject);
}
GameObject* Scene::GetObjectByName(const std::string& name) const
{
	for (GameObject* pGameObject : m_pGameObjects)
	{
		if (pGameObject)
			if (pGameObject->GetComponent<NameComponent>()->GetName() == name)
				return pGameObject;
	}
	return nullptr;
}

void Scene::CreateCamera(const std::string& name, const DirectX::XMFLOAT3& position, const bool isMainCamera)
{
	// Game Object
	GameObject* pMesh{ new GameObject(position, {}, {}, name) };

	// Camera
	CameraComponent* pCameraComponent{ new CameraComponent() };
	pCameraComponent->SetIsMainCamera(isMainCamera);

	// Adding to game object
	pMesh->AddComponent(pCameraComponent);

	// Add to scene
	AddGameObject(pMesh);

	// Set as the main camera
	if (isMainCamera)
		SetSceneCamera(pMesh);

	// Edit game object in scene
	pMesh->GetComponent<TransformComponent>()->Translate(position);
}
void Scene::CreateSceneCamera(const std::string& name, const DirectX::XMFLOAT3& position)
{
	// Game Object
	m_pSceneCameraGO = new GameObject(position, {}, {}, name);

	// Camera
	CameraComponent* pCameraComponent{ new CameraComponent() };
	pCameraComponent->SetIsMainCamera(true);

	// Camera movement
	SceneCameraMovement* pSceneCameraMovement{ new SceneCameraMovement() };

	// Adding to game object
	m_pSceneCameraGO->AddComponent(pCameraComponent);
	m_pSceneCameraGO->AddComponent(pSceneCameraMovement);

	// Edit game object in scene
	m_pSceneCameraGO->GetComponent<TransformComponent>()->Translate(position);
}
void Scene::Create3DObject(const std::string& name, const DirectX::XMFLOAT3& position, const std::string& meshPath, Material* pMaterial)
{
	// Game Object
	GameObject* pMeshGO{ new GameObject(position, {}, {}, name) };

	// Model
	ModelComponent* pModelComponent{ new ModelComponent(meshPath) };
	pModelComponent->AddMaterial(pMaterial);

	// Adding to game object
	pMeshGO->AddComponent(pModelComponent);

	// Add to scene
	AddGameObject(pMeshGO);

	// Edit game object in scene
	pMeshGO->GetComponent<TransformComponent>()->Translate(position);
}
void Scene::Create3DObject(const std::string& name, const DirectX::XMFLOAT3& position, Mesh* pMesh, Material* pMaterial)
{
	// Game Object
	GameObject* pMeshGO{ new GameObject(position, {}, {}, name) };

	// Model
	ModelComponent* pModelComponent{ new ModelComponent(pMesh) };
	pModelComponent->AddMaterial(pMaterial);

	// Adding to game object
	pMeshGO->AddComponent(pModelComponent);

	// Add to scene
	AddGameObject(pMeshGO);

	// Edit game object in scene
	pMeshGO->GetComponent<TransformComponent>()->Translate(position);
}
void Scene::CreateLineObject(const std::string& name, const DirectX::XMFLOAT3& position, Line* pLine)
{
	// Game Object
	GameObject* pLineGO{ new GameObject(position, {}, {}, name) };

	// Model
	LineComponent* pLineComponent{ new LineComponent(pLine) };
	pLineComponent->AddMaterial(new Material_GPUInstance());

	// Adding to game object
	pLineGO->AddComponent(pLineComponent);
	
	// Edit game object in scene
	pLineGO->GetComponent<TransformComponent>()->Translate(position);

	// Add to scene
	AddGameObject(pLineGO);
}

void Scene::CleanScene()
{
	if (m_Cleanup == true)
	{
		for (GameObject* pObject : m_pGameObjects)
		{
			if (pObject->GetToBeRemoved())
			{
				// Find the object
				auto object = std::find(m_pGameObjects.begin(), m_pGameObjects.end(), pObject);
				
				// Check if the object that is found is not the end of the vector
				if (object != m_pGameObjects.end())
				{
					// Calculate the distance (index position)
					auto distance{ std::distance(m_pGameObjects.begin(), object) };

					// Remove from the vector
					m_pGameObjects.erase(m_pGameObjects.begin() + distance);
				}

				DELETE_POINTER(pObject);
			}
		}

		m_Cleanup = false;
	}
}

void Scene::CreateSceneCameraInputs()
{
	InputManager* pInputManager{ Locator::GetInputManagerService() };

	MoveDirection_Flags moveDirForward{};
	moveDirForward |= MoveDirection_Flags::FORWARD;
	MoveDirection_Flags moveDirBackward{};
	moveDirBackward |= MoveDirection_Flags::BACKWARD;
	MoveDirection_Flags moveDirLeft{};
	moveDirLeft |= MoveDirection_Flags::LEFT;
	MoveDirection_Flags moveDirRight{};
	moveDirRight |= MoveDirection_Flags::RIGHT;

	pInputManager->AddBaseKeyToMap(ControllerButton::ButtonUp, KeyboardButton::W, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_FORWARD"	, new MoveCameraCommand(moveDirForward));
	pInputManager->AddBaseKeyToMap(ControllerButton::ButtonDown, KeyboardButton::S, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_BACKWARD"	, new MoveCameraCommand(moveDirBackward));
	pInputManager->AddBaseKeyToMap(ControllerButton::ButtonLeft, KeyboardButton::A, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_LEFT"		, new MoveCameraCommand(moveDirLeft));
	pInputManager->AddBaseKeyToMap(ControllerButton::ButtonRight, KeyboardButton::D, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_RIGHT"	, new MoveCameraCommand(moveDirRight));









	//pInputManager->AddBaseKeyToMap(ControllerButton::ButtonUp, KeyboardButton::W, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_FORWARD", new MoveCameraCommand(true, false, false, false));
	//pInputManager->AddBaseKeyToMap(ControllerButton::ButtonDown, KeyboardButton::S, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_BACKWARD", new MoveCameraCommand(false, true, false, false));
	//pInputManager->AddBaseKeyToMap(ControllerButton::ButtonLeft, KeyboardButton::A, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_LEFT", new MoveCameraCommand(false, false, true, false));
	//pInputManager->AddBaseKeyToMap(ControllerButton::ButtonRight, KeyboardButton::D, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_RIGHT", new MoveCameraCommand(false, false, false, true));
	pInputManager->AddBaseKeyToMap(ControllerButton::ButtonLThumbStick, KeyboardButton::NoAction, MouseButton::RMB, ButtonPressType::BUTTON_HOLD, "ROTATE", new RotateCameraCommand());
}