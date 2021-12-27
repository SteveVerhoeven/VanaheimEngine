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
	
	m_pScene_Dev = new Scene_Dev();
	pSceneManager->AddGameScene(m_pScene_Dev);

	pSceneManager->Initialize();
}
void DevGame::PostInitialize()
{
	SceneManager* pSceneManager{ Locator::GetSceneManagerService() };
	pSceneManager->PostInitialize();
}