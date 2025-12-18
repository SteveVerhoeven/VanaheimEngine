#include "VanirPCH.h"
#include "MenubarUI.h"

#include "SceneSerializer.h"
#include "Graphics.h"
#include "Scene.h"
#include "CreateNewSceneCommand.h"
#include "SaveSceneCommand.h"
#include "OpenSceneCommand.h"

MenubarUI::MenubarUI()
		  : UI("MenubarUI")
          , m_pCreateNewSceneCommand(nullptr)
          , m_pSaveSceneCommand(nullptr)
          , m_pOpenSceneCommand(nullptr)
{}
MenubarUI::~MenubarUI()
{}

void MenubarUI::Initialize(const Vanir& /*vEditor*/)
{}
void MenubarUI::Update()
{}
void MenubarUI::FixedUpdate()
{}
void MenubarUI::ShowWindow()
{}

void MenubarUI::Render(const bool useCustomTitleBar)
{
	if (useCustomTitleBar)
	{
		const ImRect menuBarRect = { ImGui::GetCursorPos(), { ImGui::GetContentRegionAvail().x + ImGui::GetCursorScreenPos().x, ImGui::GetFrameHeightWithSpacing() } };

		ImGui::BeginGroup();
		if (BeginMenuBar(menuBarRect))
		{
            MenuButtons();
		}

		EndMenuBar();
		ImGui::EndGroup();

	}
	else
	{
		if (ImGui::BeginMenuBar())
		{
            MenuButtons();
			ImGui::EndMenuBar();
		}
	}
}

bool MenubarUI::BeginMenuBar(const ImRect& barRectangle)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;
	/*if (!(window->Flags & ImGuiWindowFlags_MenuBar))
		return false;*/

	IM_ASSERT(!window->DC.MenuBarAppending);
	ImGui::BeginGroup(); // Backup position on layer 0 // FIXME: Misleading to use a group for that backup/restore
	ImGui::PushID("##menubar");

	const ImVec2 padding = window->WindowPadding;

	// We don't clip with current window clipping rectangle as it is already set to the area below. However we clip with window full rect.
	// We remove 1 worth of rounding to Max.x to that text in long menus and small windows don't tend to display over the lower-right rounded area, which looks particularly glitchy.
	ImRect bar_rect{};
	ImRect result = barRectangle;
	result.Min.x += 0;
	result.Min.y += padding.y;
	result.Max.x += 0;
	result.Max.y += padding.y;
	bar_rect = result;
	ImRect clip_rect(IM_ROUND(ImMax(window->Pos.x, bar_rect.Min.x + window->WindowBorderSize + window->Pos.x - 10.0f)), IM_ROUND(bar_rect.Min.y + window->WindowBorderSize + window->Pos.y),
		IM_ROUND(ImMax(bar_rect.Min.x + window->Pos.x, bar_rect.Max.x - ImMax(window->WindowRounding, window->WindowBorderSize))), IM_ROUND(bar_rect.Max.y + window->Pos.y));

	clip_rect.ClipWith(window->OuterRectClipped);
	ImGui::PushClipRect(clip_rect.Min, clip_rect.Max, false);

	// We overwrite CursorMaxPos because BeginGroup sets it to CursorPos (essentially the .EmitItem hack in EndMenuBar() would need something analogous here, maybe a BeginGroupEx() with flags).
	window->DC.CursorPos = window->DC.CursorMaxPos = ImVec2(bar_rect.Min.x + window->Pos.x, bar_rect.Min.y + window->Pos.y);
	window->DC.LayoutType = ImGuiLayoutType_Horizontal;
	window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
	window->DC.MenuBarAppending = true;
	ImGui::AlignTextToFramePadding();

	return true;
}
void MenubarUI::MenuButtons()
{
    FileMenu();
    EditMenu();
    WindowMenu();
    ToolMenu();
}
void MenubarUI::FileMenu()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New", "Ctrl+N"))
        {
            m_pCreateNewSceneCommand->Execute();

            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Open...", "Ctrl+O"))
        {
            m_pOpenSceneCommand->Execute();

            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Save As...", "Ctrl+S"))
        {
            m_pSaveSceneCommand->Execute();

            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Exit"))
        {
            Locator::GetInputManagerService()->QuitGame();

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndMenu();
    }
}
void MenubarUI::EditMenu()
{
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::BeginMenu("Window size"))
        {
            //Graphics* pGraphics{ Locator::GetGraphicsService() };
            if (ImGui::MenuItem("1920x1080"))
            {
                //pGraphics->SetWindowDimensions(1920, 1080);
                //pGraphics->ResizeWindow({ 1920, 1080 });
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("2560x1080"))
            {
                //pGraphics->SetWindowDimensions(2560, 1080);
                //pGraphics->ResizeWindow({ 2560, 1080 });
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Fullscreen"))
        {
            Graphics* pGraphics{ Locator::GetGraphicsService() };
            if (!pGraphics->GetIsFullscreen())
                pGraphics->SetFullscreen(true);
            else
                pGraphics->SetFullscreen(false);

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndMenu();
    }
}
void MenubarUI::WindowMenu()
{
    if (ImGui::BeginMenu("Window"))
    {
        //// Open or Close UI windows
        //// Viewport
        //ViewportUI* pViewport{ GetUI<ViewportUI>() };
        //ImGui::Checkbox("Viewport", pViewport->CanRenderUI());

        //// Inspector
        //InspectorUI* pInspector{ GetUI<InspectorUI>() };
        //ImGui::Checkbox("Inspector", pInspector->CanRenderUI());

        //// Console
        //ConsoleUI* pConsole{ GetUI<ConsoleUI>() };
        //ImGui::Checkbox("Console", pConsole->CanRenderUI());

        //// Scene hierarchy
        //HierarchyUI* pHierarchy{ GetUI<HierarchyUI>() };
        //ImGui::Checkbox("Hierarchy", pHierarchy->CanRenderUI());

        //// Camera Viewport
        //CameraViewportUI* pCameraViewportUI{ GetUI<CameraViewportUI>() };
        //ImGui::Checkbox("Camera viewport", pCameraViewportUI->CanRenderUI());

        //// Content Browser
        //ContentBrowserUI* pContentBrowserUI{ GetUI<ContentBrowserUI>() };
        //ImGui::Checkbox("Content browser", pContentBrowserUI->CanRenderUI());

        ImGui::EndMenu();
    }
}
void MenubarUI::ToolMenu()
{
    if (ImGui::BeginMenu("Tool"))
    {
        if (ImGui::MenuItem("Close scene"))
        {
            Scene* pScene = Locator::GetSceneManagerService()->GetActiveGameScene();

            std::vector<GameObject*> pGameObjects{ pScene->GetObjects() };

            const size_t objectCount{ pGameObjects.size() };
            for (size_t i{}; i < objectCount; ++i)
                pGameObjects[i]->SetRemoveFlag();

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndMenu();
    }
}
void MenubarUI::EndMenuBar()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return;
	ImGuiContext& g = *GImGui;

	// Nav: When a move request within one of our child menu failed, capture the request to navigate among our siblings.
	if (ImGui::NavMoveRequestButNoResultYet() && (g.NavMoveDir == ImGuiDir_Left || g.NavMoveDir == ImGuiDir_Right) && (g.NavWindow->Flags & ImGuiWindowFlags_ChildMenu))
	{
		// Try to find out if the request is for one of our child menu
		ImGuiWindow* nav_earliest_child = g.NavWindow;
		while (nav_earliest_child->ParentWindow && (nav_earliest_child->ParentWindow->Flags & ImGuiWindowFlags_ChildMenu))
			nav_earliest_child = nav_earliest_child->ParentWindow;
		if (nav_earliest_child->ParentWindow == window && nav_earliest_child->DC.ParentLayoutType == ImGuiLayoutType_Horizontal && (g.NavMoveFlags & ImGuiNavMoveFlags_Forwarded) == 0)
		{
			// To do so we claim focus back, restore NavId and then process the movement request for yet another frame.
			// This involve a one-frame delay which isn't very problematic in this situation. We could remove it by scoring in advance for multiple window (probably not worth bothering)
			const ImGuiNavLayer layer = ImGuiNavLayer_Menu;
			IM_ASSERT(window->DC.NavLayersActiveMaskNext & (1 << layer)); // Sanity check
			ImGui::FocusWindow(window);
			ImGui::SetNavID(window->NavLastIds[layer], layer, 0, window->NavRectRel[layer]);
			g.NavDisableHighlight = true; // Hide highlight for the current frame so we don't see the intermediary selection.
			g.NavDisableMouseHover = g.NavMousePosDirty = true;
			ImGui::NavMoveRequestForward(g.NavMoveDir, g.NavMoveClipDir, g.NavMoveFlags, g.NavMoveScrollFlags); // Repeat
		}
	}

	IM_MSVC_WARNING_SUPPRESS(6011); // Static Analysis false positive "warning C6011: Dereferencing NULL pointer 'window'"
	// IM_ASSERT(window->Flags & ImGuiWindowFlags_MenuBar); // NOTE(Yan): Needs to be commented out because Jay
	IM_ASSERT(window->DC.MenuBarAppending);
	ImGui::PopClipRect();
	ImGui::PopID();
	window->DC.MenuBarOffset.x = window->DC.CursorPos.x - window->Pos.x; // Save horizontal position so next append can reuse it. This is kinda equivalent to a per-layer CursorPos.
	g.GroupStack.back().EmitItem = false;
	ImGui::EndGroup(); // Restore position on layer 0
	window->DC.LayoutType = ImGuiLayoutType_Vertical;
	window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
	window->DC.MenuBarAppending = false;
}