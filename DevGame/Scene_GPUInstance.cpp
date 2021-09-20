#include "pch.h"
#include "Scene_GPUInstance.h"

#include <InputManager.h>
#include <MoveCameraCommand.h>
#include <RotateCameraCommand.h>

#include <Material_GPUInstance.h>

Scene_GPUInstance::Scene_GPUInstance()
				  : Scene()
{}
Scene_GPUInstance::~Scene_GPUInstance()
{}

void Scene_GPUInstance::Initialize()
{
	// Bunny 1
	const std::string name1{ "Bunny 1" };
	const DirectX::XMFLOAT3 pos1{ 10, 0, 0 };
	Material_GPUInstance* pMaterial1 = new Material_GPUInstance();
	Create3DObject(name1, pos1, "./Resources/Meshes/lowpoly_bunny.obj",pMaterial1);

	// Bunny 2
	const std::string name2{ "Bunny 1" };
	const DirectX::XMFLOAT3 pos2{ -10, 0, 0 };
	Material_GPUInstance* pMaterial2 = new Material_GPUInstance();
	Create3DObject(name2, pos2, "./Resources/Meshes/lowpoly_bunny.obj", pMaterial2);

	// Main camera
	const std::string name3{ "Camera-Main" };
	const DirectX::XMFLOAT3 pos3{ 0, 0, -20 };
	CreateCamera(name3, pos3);

	// Inputs
	//CreateInputs();

	ActivateScene();
	Scene::Initialize();
}
void Scene_GPUInstance::PostInitialize()
{
	Scene::PostInitialize();
}
void Scene_GPUInstance::Update(const float elapsedSec)
{
	Scene::Update(elapsedSec);
}
void Scene_GPUInstance::FixedUpdate(const float timeEachUpdate)
{
	Scene::FixedUpdate(timeEachUpdate);
}
void Scene_GPUInstance::LateUpdate()
{
	Scene::LateUpdate();
}
void Scene_GPUInstance::Render() const
{
	Scene::Render();
}

void Scene_GPUInstance::CreateInputs()
{
	InputManager* pInputManager{ Locator::GetInputManagerService() };
	// Arrows
	pInputManager->AddKeyToMap(ControllerButton::ButtonUp	, KeyboardButton::ARROW_UP, ButtonPressType::BUTTON_RELEASED, "MOVE_FORWARD", new MoveCameraCommand(true, false, false, false));
	pInputManager->AddKeyToMap(ControllerButton::ButtonDown	, KeyboardButton::ARROW_DOWN, ButtonPressType::BUTTON_RELEASED, "MOVE_BACKWARD", new MoveCameraCommand(false, true, false, false));
	pInputManager->AddKeyToMap(ControllerButton::ButtonLeft	, KeyboardButton::ARROW_LEFT, ButtonPressType::BUTTON_RELEASED, "MOVE_LEFT", new MoveCameraCommand(false, false, true, false));
	pInputManager->AddKeyToMap(ControllerButton::ButtonRight, KeyboardButton::ARROW_RIGHT, ButtonPressType::BUTTON_RELEASED, "MOVE_RIGHT", new MoveCameraCommand(false, false, false, true));

	// WASD
	pInputManager->AddKeyToMap(ControllerButton::ButtonUp			, KeyboardButton::W, ButtonPressType::BUTTON_RELEASED, "MOVE_FORWARD", new MoveCameraCommand(true, false, false, false));
	pInputManager->AddKeyToMap(ControllerButton::ButtonDown, KeyboardButton::S, ButtonPressType::BUTTON_RELEASED, "MOVE_BACKWARD", new MoveCameraCommand(false, true, false, false));
	pInputManager->AddKeyToMap(ControllerButton::ButtonLeft, KeyboardButton::A, ButtonPressType::BUTTON_RELEASED, "MOVE_LEFT", new MoveCameraCommand(false, false, true, false));
	pInputManager->AddKeyToMap(ControllerButton::ButtonRight, KeyboardButton::D, ButtonPressType::BUTTON_RELEASED, "MOVE_RIGHT", new MoveCameraCommand(false, false, false, true));
	pInputManager->AddKeyToMap(ControllerButton::ButtonLThumbStick, KeyboardButton::E, ButtonPressType::BUTTON_HOLD, "ROTATE", new RotateCameraCommand());
}