#include "pch.h"
#include "DevGame.h"

#include <SceneManager.h>
#include "Scene_Dev.h"

DevGame::DevGame(const std::string& name)
		: Game(name)
		, m_pScene_Dev(nullptr)
{}

void DevGame::Initialize(HINSTANCE hInstance)
{
	Game::Initialize(hInstance);	
	
	SceneManager* pSceneManager{ Locator::GetSceneManagerService() };
	
	#ifdef DEVELOPMENT
	m_pScene_Dev = new Scene_Dev();
	pSceneManager->AddGameScene(m_pScene_Dev);
	#endif // DEVELOPMENT
	
	#ifdef GPUINSTANCING
	m_pScene_GPUInstance = new Scene_GPUInstance();
	pSceneManager->AddGameScene(m_pScene_GPUInstance);
	#endif // GPUINSTANCING
	
	#ifdef PROCEDURAL_GENERATION
	m_pScene_ProcGen = new Scene_ProcGen();
	pSceneManager->AddGameScene(m_pScene_ProcGen);
	#endif // PROCEDURAL_GENERATION

	pSceneManager->Initialize();
}
void DevGame::PostInitialize()
{
	SceneManager* pSceneManager{ Locator::GetSceneManagerService() };
	pSceneManager->PostInitialize();
}