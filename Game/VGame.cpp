#include "GamePCH.h"
#include "VGame.h"

#include <SceneManager.h>
#include "Scene_Game.h"
#include "Scene_ObjectLoading.h"

VGame::VGame(const std::string& name)
	  : Application(name)
{}

void VGame::Initialize()
{
	//#define ProcGen
	#define ObjectLoading

	SceneManager* pSceneManager{ Locator::GetSceneManagerService() };
	#ifdef ProcGen
	Scene_Game* pScene_ProcGen = new Scene_Game();
	pSceneManager->AddGameScene(pScene_ProcGen);
	#endif
	#ifdef ObjectLoading
	Scene_ObjectLoading* pScene_ObjectLoading = new Scene_ObjectLoading();
	pSceneManager->AddGameScene(pScene_ObjectLoading);
	#endif

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