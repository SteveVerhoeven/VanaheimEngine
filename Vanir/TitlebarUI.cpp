#include "VanirPCH.h"
#include "TitlebarUI.h"

#include "MenubarUI.h"
#include "Window.h"

TitlebarUI::TitlebarUI()
	       : UI("TitlebarUI")
	       , m_TitleBarHovered(false)
	       , m_pLogoIcon(nullptr)
	       , m_pMinIcon(nullptr)
	       , m_pMaxIcon(nullptr)
	       , m_pCloseIcon(nullptr)
{}
TitlebarUI::~TitlebarUI()
{}

void TitlebarUI::Initialize(const Vanir& /*vEditor*/)
{
    ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };

    m_pLogoIcon = pResourceManager->LoadTexture("Resources/Icon/VanaheimSceneIcon.png", true);
    m_pMinIcon = pResourceManager->LoadTexture("Resources/Icon/MinimizeIcon.png", true);
    m_pMaxIcon = pResourceManager->LoadTexture("Resources/Icon/MaximizeIcon.png", true);
    m_pCloseIcon = pResourceManager->LoadTexture("Resources/Icon/CloseIcon.png", true);
}
void TitlebarUI::Update()
{}
void TitlebarUI::FixedUpdate()
{}
void TitlebarUI::ShowWindow()
{}

void TitlebarUI::Render(const bool drawCustomTitleBar, float& outTitlebarHeight, MenubarUI* pMenuBarUI)
{
    const float titlebarHeight = 58.0f;
    const bool isMaximized = Locator::GetWindowService()->GetIsFullScreen();//IsMaximized();
    float titlebarVerticalOffset = isMaximized ? -6.0f : 0.0f;
    const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

    ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset));
    const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
    const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
                                 ImGui::GetCursorScreenPos().y + titlebarHeight };
    auto* bgDrawList = ImGui::GetBackgroundDrawList();
    //auto* fgDrawList = ImGui::GetForegroundDrawList();
    bgDrawList->AddRectFilled(titlebarMin, titlebarMax, IM_COL32(21, 21, 21, 255));

    Logo(windowPadding, titlebarVerticalOffset);

    ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

    static float moveOffsetX;
    static float moveOffsetY;
    const float w = ImGui::GetContentRegionAvail().x;
    const float buttonsAreaWidth = 94;

    // Title bar drag area
    // On Windows we hook into the GLFW win32 window internals
    ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset)); // Reset cursor pos
    ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttonsAreaWidth, titlebarHeight));

    m_TitleBarHovered = ImGui::IsItemHovered();

    if (isMaximized)
    {
        float windowMousePosY = ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y;
        if (windowMousePosY >= 0.0f && windowMousePosY <= 5.0f)
            m_TitleBarHovered = true; // Account for the top-most pixels which don't register
    }

    MenuBar(drawCustomTitleBar, titlebarVerticalOffset, windowPadding, pMenuBarUI);

    Title(windowPadding);

    // Window buttons
    const ImU32 buttonColN = IM_COL32(192 * 0.9f, 192 * 0.9f, 192 * 0.9f, 255);
    //const ImU32 buttonColH = IM_COL32(192 * 1.2f, 192 * 1.2f, 192 * 1.2f, 255);
    //const ImU32 buttonColP = IM_COL32(128, 128, 128, 255);
    const ImVec2 buttonSize{ 14.0f * 2, 14.0f * 2 };
    const ImVec2 positionOffset{ 14.0f * 2, 14.0f * 2 };


    MinimizeButton(buttonSize, ImVec2{-20, 12}, buttonColN);
    MaximizeButton(buttonSize, ImVec2{ -5, 12}, buttonColN, isMaximized);
    CloseButton(buttonSize, ImVec2{ -5, 12 }, buttonColN);

    ImGui::Spring(-1.0f, 18.0f);
    ImGui::EndHorizontal();

    outTitlebarHeight = titlebarHeight;
}

void TitlebarUI::Logo(const ImVec2& windowPadding, const float& titlebarVerticalOffset)
{
    const int logoWidth = 64;// m_LogoTex->GetWidth();
    const int logoHeight = 64;// m_LogoTex->GetHeight();
    const ImVec2 logoOffset(7.5f + windowPadding.x, windowPadding.y + titlebarVerticalOffset);
    const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
    const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
    auto* fgDrawList = ImGui::GetForegroundDrawList();
    fgDrawList->AddImage(m_pLogoIcon->GetShaderResourceView(), logoRectStart, logoRectMax);
}

void TitlebarUI::MenuBar(const bool drawCustomTitleBar, const float& titlebarVerticalOffset, const ImVec2& windowPadding, MenubarUI* pMenuBarUI)
{
    // Draw Menubar
    if (true)
    {
        ImGui::SuspendLayout();
        {
            ImGui::SetItemAllowOverlap();
            const float logoHorizontalOffset = 16.0f * 2.0f + 48.0f + windowPadding.x;
            ImGui::SetCursorPos(ImVec2(logoHorizontalOffset, 6.0f + titlebarVerticalOffset));
            pMenuBarUI->Render(drawCustomTitleBar);

            if (ImGui::IsItemHovered())
                m_TitleBarHovered = false;
        }

        ImGui::ResumeLayout();
    }
}

void TitlebarUI::Title(const ImVec2& windowPadding)
{
    // Centered Window title
    ImVec2 currentCursorPos = ImGui::GetCursorPos();
    ImVec2 textSize = ImGui::CalcTextSize("Vanaheim");
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - textSize.x * 0.5f, 2.0f + windowPadding.y + 6.0f));
    float oldScale = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale *= 2;
    ImGui::PushFont(ImGui::GetFont());
    ImGui::Text("%s", "Vanaheim"); // Draw title
    ImGui::GetFont()->Scale = oldScale;
    ImGui::PopFont();
    ImGui::SetCursorPos(currentCursorPos);
}
void TitlebarUI::MinimizeButton(const ImVec2& size, const ImVec2& posOffset, const ImU32& buttonColorNormal) const
{
    // Minimize Button
    ImGui::Spring();
    ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + posOffset.x, ImGui::GetCursorPosY() + posOffset.y });
    {
        if (ImGui::InvisibleButton("Minimize", ImVec2(size.x, size.y)))
        {
            Locator::GetWindowService()->MinimizeWindow();
        }

        ImDrawList* pDrawList = ImGui::GetForegroundDrawList();
        pDrawList->AddImage(m_pMinIcon->GetShaderResourceView(), ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), buttonColorNormal);
    }
}
void TitlebarUI::MaximizeButton(const ImVec2& size, const ImVec2& posOffset, const ImU32& buttonColorNormal, const bool isMaximized) const
{
    // Maximize Button
    ImGui::Spring(-1.0f, 17.0f);
    ImGui::SetCursorPos(ImVec2{ ImGui::GetCursorPosX() + posOffset.x, ImGui::GetCursorPosY() + posOffset.y });
    {
        if (ImGui::InvisibleButton("Maximize", ImVec2(size.x, size.y)))
        {
            if (isMaximized)
                Locator::GetWindowService()->MaximizeWindow(true);
            else
                Locator::GetWindowService()->MaximizeWindow(false);
        }

        ImDrawList* pDrawList = ImGui::GetForegroundDrawList();
        pDrawList->AddImage(m_pMaxIcon->GetShaderResourceView(), ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), buttonColorNormal);

    }
}
void TitlebarUI::CloseButton(const ImVec2& size, const ImVec2& posOffset, const ImU32& buttonColorNormal) const
{
    // Close Button
    ImGui::Spring(-1.0f, 15.0f);
    //UI::ShiftCursorY(8.0f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + posOffset.x);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + posOffset.y);
    {
        //const int iconWidth = m_IconClose->GetWidth();
        //const int iconHeight = m_IconClose->GetHeight();
        if (ImGui::InvisibleButton("Close", ImVec2(size.x, size.y)))
        {
            Locator::GetInputManagerService()->QuitGame();
        }

        auto* drawList = ImGui::GetForegroundDrawList();
        drawList->AddImage(m_pCloseIcon->GetShaderResourceView(), ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), buttonColorNormal);
    }
}