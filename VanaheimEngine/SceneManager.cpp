#include "VanaheimPCH.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Graphics.h"
#include "Application.h"

//#include "../Vanir/InspectorUI.h"
//#include "../Vanir/Vanir.h"

SceneManager::SceneManager()
			 : m_pGameScenes(std::vector<Scene*>())
			 , m_pMenuScenes(std::vector<Scene*>())
			 , m_ActiveGameSceneIndex()
			 , m_ActiveMenuSceneIndex()
			 , m_MenuActive(true)
			 , m_GameActive(false)
			 , m_pGraphics(nullptr)
			 , m_pUIManager(nullptr)
{}
SceneManager::~SceneManager()
{
	for (Scene* pScene : m_pMenuScenes)
		DELETE_POINTER(pScene);

	for (Scene* pScene : m_pGameScenes)
		DELETE_POINTER(pScene);

	m_pMenuScenes.clear();
	m_pGameScenes.clear();
}

void SceneManager::Initialize()
{
	m_pGraphics = Locator::GetGraphicsService();

	for (Scene* pScene : m_pMenuScenes)
		pScene->Initialize();

	for (Scene* pScene : m_pGameScenes)
		pScene->Initialize();
}
void SceneManager::PostInitialize()
{
	for (Scene* pScene : m_pMenuScenes)
		pScene->PostInitialize();

	for (Scene* pScene : m_pGameScenes)
		pScene->PostInitialize();
}
void SceneManager::Update(const float elapsedSec)
{
	for (Scene* pScene : m_pMenuScenes)
		pScene->Update(elapsedSec);

	for (Scene* pScene : m_pGameScenes)
		pScene->Update(elapsedSec);
}
void SceneManager::FixedUpdate(const float timeEachUpdate)
{
	for (Scene* pScene : m_pMenuScenes)
		pScene->FixedUpdate(timeEachUpdate);

	for (Scene* pScene : m_pGameScenes)
		pScene->FixedUpdate(timeEachUpdate);
}
void SceneManager::LateUpdate()
{
	for (Scene* pScene : m_pMenuScenes)
		pScene->LateUpdate();

	for (Scene* pScene : m_pGameScenes)
		pScene->LateUpdate();
}
void SceneManager::Render(Application& vEditor)
{	
	m_pGraphics->ClearBackbuffer();
	vEditor.Prerender();
	
	SetSceneCameraAsMain();
	m_pGraphics->SetGameRenderTarget();		// Set the render target for the viewport in the UI
	for (Scene* pScene : m_pMenuScenes)
		pScene->Render();
	
	for (Scene* pScene : m_pGameScenes)
		pScene->Render();
	
	if (vEditor.PreSideCamRender())
	{
		m_pGraphics->SetCameraRenderTarget();	// Set the render target for the camera viewport in the UI
		for (Scene* pScene : m_pMenuScenes)
			pScene->Render();

		for (Scene* pScene : m_pGameScenes)
			pScene->Render();
		
		SetSceneCameraAsMain();
	}
	
	m_pGraphics->SetMainRenderTarget();		// Set the render target for the entire program window
	vEditor.Render();

	vEditor.Postrender();
	m_pGraphics->PresentBackbuffer();
}

void SceneManager::DeactivateAllGameScenes()
{
	for (Scene* pScene : m_pGameScenes)
		pScene->DeactivateScene();

	m_GameActive = false;
}
void SceneManager::DeactivateAllMenuScenes()
{
	for (Scene* pScene : m_pMenuScenes)
		pScene->DeactivateScene();

	m_MenuActive = false;
}

void SceneManager::ActivateNextGameScene()
{
	// Deactivate current scene
	m_pGameScenes[size_t(m_ActiveGameSceneIndex)]->DeactivateScene();

	// Increment index that remembers which scene the game is on
	m_ActiveGameSceneIndex++;

	// Activate the new current scene
	m_pGameScenes[size_t(m_ActiveGameSceneIndex)]->ActivateScene();
}

void SceneManager::ActivateGameSceneByIndex(const size_t sceneIndex)
{
	m_pGameScenes[sceneIndex]->ActivateScene();
	m_ActiveGameSceneIndex = int(sceneIndex);

	m_GameActive = true;
}
void SceneManager::ActivateMenuSceneByIndex(const size_t sceneIndex)
{
	m_pMenuScenes[sceneIndex]->ActivateScene();
	m_ActiveMenuSceneIndex = int(sceneIndex);

	m_MenuActive = true;
}

Scene* SceneManager::ReplaceCurrentGameSceneByNewOne()
{
	// Old scene
	Scene* pCurrentScene{ GetActiveGameScene() };

	// New Scene
	Scene* pNewScene{ new Scene() };
	m_pGameScenes.emplace_back(pNewScene);

	// Get old active scene & deactivate that one
	pCurrentScene->DeactivateScene();

	// Set scene camera
	GameObject* pCameraGameObject{ pCurrentScene->GetSceneCamera() };
	pNewScene->SetSceneCamera(pCameraGameObject);
	Locator::ProvideRenderCameraService(pCameraGameObject->GetComponent<CameraComponent>());

	// Destroy old active scene
	//std::vector<Scene*>::iterator result = std::find(m_pGameScenes.begin(), m_pGameScenes.end(), pCurrentScene);
	//m_pGameScenes.erase(result);
	//DELETE_POINTER(pCurrentScene);
	DestroyOldGameScene(pCurrentScene);

	// Activate new scene
	//SetActiveGameSceneIndex(m_pGameScenes.size() - 1);
	//pNewScene->ActivateScene();
	ActivateNewScene(pNewScene);

	// Delete Resources from old scene
	Locator::GetResourceManagerService()->ClearResources(true);

	return m_pGameScenes[m_ActiveGameSceneIndex];
}

Scene* SceneManager::CreateNewGameScene()
{
	// New Scene
	Scene* pNewScene{ new Scene() };
	m_pGameScenes.emplace_back(pNewScene);

	return pNewScene;
}

void SceneManager::DestroyOldGameScene(Scene* pScene)
{
	std::vector<Scene*>::iterator result = std::find(m_pGameScenes.begin(), m_pGameScenes.end(), pScene);
	m_pGameScenes.erase(result);
	DELETE_POINTER(pScene);
}

void SceneManager::ActivateNewScene(Scene* pToActivateScene)
{
	SetActiveGameSceneIndex(m_pGameScenes.size() - 1);
	pToActivateScene->ActivateScene();
}

void SceneManager::SetSceneCameraAsMain()
{
	Scene* pScene{ GetActiveGameScene() };
	GameObject* pCameraObject{ pScene->GetSceneCamera() };

	Locator::ProvideRenderCameraService(pCameraObject->GetComponent<CameraComponent>());
}