#include "VanaheimPCH.h"
#pragma comment(lib, "XInput.lib")
#include "InputManager.h"

#include "Command.h"
#include "RotateCameraCommand.h"
#include "Window.h"
#include "Mouse.h"
#include "SceneSerializer.h"

#include "../Vanir/HierarchyUI.h"
//#include "../Vanir/Vanir.h"

InputManager::InputManager()
			 : m_ControllerState()
			 , m_pInputs(std::vector<InputData*>())
			 , m_pMouse(new Mouse())
			 , m_QuitGame(false)
{}
InputManager::~InputManager()
{
	for (InputData* pInputData : m_pInputs)
		DELETE_POINTER(pInputData);
	m_pInputs.clear();
	for (InputData* pInputData : m_pBaseInputs)
		DELETE_POINTER(pInputData);
	m_pBaseInputs.clear();

	DELETE_POINTER(m_pMouse);
}

MSG InputManager::ProcessInput(const float /*elapsedSec*/)
{
	MSG msg{};
	ZeroMemory(&msg, sizeof(msg));

	if (!ProcessWindowsEvents(msg) || 
		!ProcessGameInput(msg) ||
		!ProcessUIEvents() ||
		m_QuitGame || 
		msg.message == WM_QUIT)
	{
		msg.message = WM_QUIT;
		m_QuitGame = true;
		return msg;
	}

	// Update mouse movement
	m_pMouse->ProcessMouseInputs();

	return msg;
}
bool InputManager::ProcessWindowsEvents(MSG& msg)
{
	// PeekMessage - Reference: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-peekmessagea
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			return false;

		ProcessMouseEvents(msg);
	}

	return true;
}
bool InputManager::ProcessUIEvents()
{
	const size_t numberOfInputs{ m_pBaseInputs.size() };
	for (size_t i{}; i < numberOfInputs; ++i)
	{
		InputData* pInputData{ m_pBaseInputs[i] };
		const std::vector<KeyboardButton>& keys{ pInputData->keyData.keyboardButtons };

		if (keys.size() > 1 && IsKeyDown(int(keys[0])) && IsKeyDown(int(keys[1])))
		{
			pInputData->commandData.pCommand->Execute();
		}
	}

	//if (IsKeyDown(int(KeyboardButton::L_CTRL)) && IsKeyDown(int(KeyboardButton::N)))
	//{
	//	Locator::GetSceneManagerService()->ReplaceCurrentGameSceneByNewOne();
	//}
	//if (IsKeyDown(int(KeyboardButton::L_CTRL)) && IsKeyDown(int(KeyboardButton::S)))
	//{
	//	const std::string filePath{ WindowsUtils::FileDialogs::SaveFile("Vanaheim Scene (*.Vanaheim)\0*.Vanaheim\0") };

	//	if (!filePath.empty())
	//	{
	//		SceneSerializer serializer{};
	//		serializer.Serialize(filePath, Locator::GetSceneManagerService()->GetActiveGameScene());
	//	}
	//}
	//if (IsKeyDown(int(KeyboardButton::L_CTRL)) && IsKeyDown(int(KeyboardButton::O)))
	//{
	//	const std::string filePath{ WindowsUtils::FileDialogs::OpenFile("Vanaheim Scene (*.Vanaheim)\0*.Vanaheim\0") };

	//	if (!filePath.empty())
	//	{
	//		Scene* pScene{ Locator::GetSceneManagerService()->ReplaceCurrentGameSceneByNewOne() };

	//		SceneSerializer serializer{};
	//		serializer.Deserialize(filePath, pScene, nullptr); // TODO: Fix inspectorUI
	//	}
	//}

	return true;
}
bool InputManager::ProcessGameInput(MSG& msg)
{
	// PeekMessage - Reference: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-peekmessagea
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		for (const InputData* pInputData : m_pInputs)
		{
			bool executeCommand{ false };
			// ******************************************************************
			// Asking the state of the registered input key for the current scene
			// ******************************************************************
			/* GetKeyState - parameters */
			int nVirtKeyboardKey{ int(pInputData->keyData.keyboardButtons[0]) };
			int nVirtMouseKey{ int(pInputData->keyData.mouseButton) };
			// Reference: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate
			
			bool commandButtonPressed{ (GetKeyState(nVirtKeyboardKey) & 0x8000) != 0 };
			if (pInputData->engineKeyInput == true)
			{
				//bool RMBPressed{ (GetKeyState(nVirtMouseKey) & 0x8000) != 0 };
				bool RMBPressed{ IsKeyUp(nVirtMouseKey) };
				if (RMBPressed && commandButtonPressed)
				{
					executeCommand = true;
					LOG_INFO("KeyDown");
				}
			}
			else
			{
				if (commandButtonPressed)
					executeCommand = true;
			}
			

			if (executeCommand)
			{
				Command* pCommand{ GetCommand(pInputData->keyData.keyboardButtons[0]) };
				pCommand->Execute();
				 
				return true;
			}
		}
	}

	return true;
}

void InputManager::AddBaseKeyToMap(const ControllerButton& cButton, const KeyboardButton& kButton, const MouseButton& mButton, const ButtonPressType& pressType, const std::string& name, Command* const pCommand)
{
	InputData* pInputData{ new InputData() };
	KeyData keyData{};
	keyData.buttonState = pressType;
	keyData.controllerButton = cButton;
	keyData.keyboardButtons = { kButton };
	keyData.mouseButton = mButton;
	CommandData commandData{};
	commandData.commandName = name;
	commandData.pCommand = pCommand;

	pInputData->engineKeyInput = true;
	pInputData->commandData = commandData;
	pInputData->keyData = keyData;

	m_pBaseInputs.push_back(pInputData);
}
void InputManager::AddBaseKeyComboToMap(const std::vector<KeyboardButton>& kButtons, const std::string& name, Command* const pCommand)
{
	InputData* pInputData{ new InputData() };
	KeyData keyData{};
	keyData.buttonState = ButtonPressType::BUTTON_PRESSED;
	keyData.controllerButton = ControllerButton::NoAction;
	keyData.keyboardButtons = kButtons;
	keyData.mouseButton = MouseButton::NoAction;
	CommandData commandData{};
	commandData.commandName = name;
	commandData.pCommand = pCommand;

	pInputData->engineKeyInput = true;
	pInputData->commandData = commandData;
	pInputData->keyData = keyData;

	m_pBaseInputs.push_back(pInputData);
}
void InputManager::AddKeyToMap(const ControllerButton& cButton, const KeyboardButton& kButton, const MouseButton& mButton, const ButtonPressType& pressType, const std::string& name, Command* const pCommand)
{
	InputData* pInputData{ new InputData() };
	KeyData keyData{};
	keyData.buttonState = pressType;
	keyData.controllerButton = cButton;
	keyData.keyboardButtons = { kButton };
	keyData.mouseButton = mButton;
	CommandData commandData{};
	commandData.commandName = name;
	commandData.pCommand = pCommand;

	pInputData->engineKeyInput = false;
	pInputData->commandData = commandData;
	pInputData->keyData = keyData;

	m_pInputs.push_back(pInputData);
}

bool InputManager::IsKeyDown(const int keyValue) const
{
	return (GetKeyState(keyValue) & 0x8000) > 0;
}
bool InputManager::IsKeyUp(const int keyValue) const
{
	return (GetKeyState(keyValue) & 0x8000) < 0;
}


// Retrieve the command from a Controller button
Command* InputManager::GetCommand(const ControllerButton& cButton)
{
	auto result = std::find_if(m_pBaseInputs.begin(), m_pBaseInputs.end(), [&](InputData* pInputData)
	{ return pInputData->keyData.controllerButton == cButton; });

	if (result != m_pBaseInputs.end())
		return (*result)->commandData.pCommand;

	return nullptr;
}
// Retrieve the command from a Keyboard button
Command* InputManager::GetCommand(const KeyboardButton& kButton)
{
	auto result = std::find_if(m_pBaseInputs.begin(), m_pBaseInputs.end(), [&](InputData* pInputData)
	{ return pInputData->keyData.keyboardButtons[0] == kButton; });

	if (result != m_pBaseInputs.end())
		return (*result)->commandData.pCommand;

	return nullptr;
}
Command* InputManager::GetCommand(const MouseButton& mButton)
{
	auto result = std::find_if(m_pBaseInputs.begin(), m_pBaseInputs.end(), [&](InputData* pInputData)
		{ return pInputData->keyData.mouseButton == mButton; });

	if (result != m_pBaseInputs.end())
		return (*result)->commandData.pCommand;

	return nullptr;
}

void InputManager::ProcessMouseEvents(MSG& msg)
{
	if (msg.wParam == 'G' && msg.message == WM_KEYUP)
	{
		if (m_pMouse->GetMouseActive())
			m_pMouse->DeactivateMouseInput();
		else
			m_pMouse->ActivateMouseInput();
	}

	// Mouse rotation
	if (m_pMouse->GetMouseActive() && msg.message == WM_MOUSEMOVE)
	{
		POINT mouseMov{ m_pMouse->GetMouseMovement() };

		DirectX::XMFLOAT2 mouse{};
		mouse.x = float(mouseMov.x);
		mouse.y = float(mouseMov.y);

		Command* pCommand{ GetCommand(ControllerButton::ButtonLThumbStick) };
		RotateCameraCommand* pRotateCommand{ dynamic_cast<RotateCameraCommand*>(pCommand) };
		pRotateCommand->SetMouseMovement(mouse);
	}
}