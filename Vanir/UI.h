#pragma once
#include "../Libs/IMGUI/imgui.h"
#include "../Libs/IMGUI/imgui_internal.h"
#include <vector>
#include <string>

#include "Subject.h"

enum class UIButtonType
{
	ARROW_BUTTON,
	SLIDER_FLOAT,
	SLIDER_INT,
	CHECKBOX,
	LIST
};

class Vanir;
class UI
{		
	public:
		UI(const std::string& windowName, const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size);
		virtual ~UI() = default;

		UI(const UI&) = delete;
		UI& operator=(const UI&) = delete;
		UI(UI&&) = delete;
		UI& operator=(UI&&) = delete;

		virtual void Initialize(const Vanir& vEditor) = 0;
		virtual void Update() = 0;
		virtual void FixedUpdate() = 0;
		virtual void ShowWindow() = 0;
		
		void ActivateUI() { m_RenderUI = true; }
		void DeactivateUI() { m_RenderUI = false; }

		bool* CanRenderUI() { return &m_RenderUI; }

	protected:
		bool m_RenderUI;
		std::string m_Name;
		DirectX::XMFLOAT2 m_Position;
		DirectX::XMFLOAT2 m_Size;

		void BeginWindowBase();
		void BeginWindowBase(const ImGuiWindowFlags& windowFlags);
		void EndWindowBase();
		
		void PushStyle_RemovePadding();
		void PopStyleVar();
		
	private:
};