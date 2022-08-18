#pragma once
#include <string>
#include <vector>
#include <XInput.h>
#include "InputData.h"
#include <unordered_map>

class Mouse;
class Command;
class InputManager final
{
	public:
		InputManager();
		~InputManager();

		MSG ProcessInput(const float elapsedSec);

		void AddBaseKeyToMap(const ControllerButton& cButton, 
							 const KeyboardButton& kButton, 
							 const MouseButton& mButton,
							 const ButtonPressType& pressType, 
							 const std::string& name, 
							 Command* const pCommand);
		void AddBaseKeyComboToMap(const std::vector<KeyboardButton>& kButtons,
							      const std::string& name,
							      Command* const pCommand);
		void AddKeyToMap(const ControllerButton& cButton, 
						 const KeyboardButton& kButton, 
						 const MouseButton& mButton,
						 const ButtonPressType& pressType, 
						 const std::string& name, 
						 Command* const pCommand);

		bool GetQuitGame() const { return m_QuitGame; }
		void QuitGame() { m_QuitGame = true; }

		Mouse* GetMouse() const { return m_pMouse; }

		bool IsKeyDown(const int keyValue) const;
		bool IsKeyUp(const int keyValue) const;

		// Retrieve the command from a Controller button
		Command* GetCommand(const ControllerButton& cButton);
		// Retrieve the command from a Keyboard button
		Command* GetCommand(const KeyboardButton& kButton);
		// Retrieve the command from a Keyboard button
		Command* GetCommand(const MouseButton& mButton);

	private:
		bool m_QuitGame;
		Mouse* m_pMouse;

		// Information about the controller/keyboard
		XINPUT_STATE m_ControllerState;

		// General
		std::vector<InputData*> m_pBaseInputs;
		std::vector<InputData*> m_pInputs;

		bool ProcessWindowsEvents(MSG& msg);
		bool ProcessUIEvents();
		bool ProcessGameInput(MSG& msg);
				
		//
		void ProcessMouseEvents(MSG& msg);
};