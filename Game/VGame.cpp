#include "GamePCH.h"
#include "VGame.h"

#include <SceneManager.h>
#include "Scene_Game.h"

VGame::VGame(const std::string& name)
	  : Application(name)
{}

void VGame::Initialize()
{
	SceneManager* pSceneManager{ Locator::GetSceneManagerService() };
	Scene_Game* pScene_ProcGen = new Scene_Game();
	pSceneManager->AddGameScene(pScene_ProcGen);

	pSceneManager->Initialize();
}
void VGame::PostInitialize()
{
	SceneManager* pSceneManager{ Locator::GetSceneManagerService() };
	pSceneManager->PostInitialize();
}

void VGame::Update()
{}
void VGame::FixedUpdate()
{}

void VGame::Prerender()
{}
bool VGame::PreSideCamRender()
{
	return false;
}
void VGame::Render()
{}
void VGame::Postrender()
{}