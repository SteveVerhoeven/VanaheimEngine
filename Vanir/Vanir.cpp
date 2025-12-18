#include "VanirPCH.h"
#include "Vanir.h"

#include <SceneManager.h>
#include <GeneratorManager.h>
#include <NoiseGenerator.h>

#include <Timer.h>

// ImGui
#include "backends\imgui_impl_dx11.h"
#include "backends\imgui_impl_win32.h"

// Vanaheim Engine
#include "Window.h"
#include "Graphics.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "UI.h"

#include "SaveSceneCommand.h"
#include "OpenSceneCommand.h"
#include "CreateNewSceneCommand.h"

#include "MoveCameraCommand.h"
#include "RotateCameraCommand.h"

Vanir::Vanir(const std::string& name)
      : Application(name)
      , m_EngineFullScreen(false)
      , m_pTitlebarUI(nullptr)
      , m_pMenubarUI(nullptr)
      , m_pWindow(nullptr)
      , m_pMainViewport(nullptr)
      , m_pUIs(std::vector<UI*>())
{}
Vanir::~Vanir()
{
    ShutdownImGui();
    DELETE_POINTERS(m_pUIs, m_pUIs.size());
}

void Vanir::Initialize()
{
    m_pWindow = Locator::GetWindowService();
    Graphics* pGraphics{ Locator::GetGraphicsService() };

    InitializeImGui(m_pWindow, pGraphics);

    // Scene cam inputs
    {
        InputManager* pInputManager{ Locator::GetInputManagerService() };

        MoveDirection_Flags moveDirForward{};
        moveDirForward |= MoveDirection_Flags::FORWARD;
        MoveDirection_Flags moveDirBackward{};
        moveDirBackward |= MoveDirection_Flags::BACKWARD;
        MoveDirection_Flags moveDirLeft{};
        moveDirLeft |= MoveDirection_Flags::LEFT;
        MoveDirection_Flags moveDirRight{};
        moveDirRight |= MoveDirection_Flags::RIGHT;

        pInputManager->AddBaseKeyToMap(ControllerButton::ButtonUp, KeyboardButton::W, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_FORWARD", new MoveCameraCommand(moveDirForward));
        pInputManager->AddBaseKeyToMap(ControllerButton::ButtonDown, KeyboardButton::S, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_BACKWARD", new MoveCameraCommand(moveDirBackward));
        pInputManager->AddBaseKeyToMap(ControllerButton::ButtonLeft, KeyboardButton::A, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_LEFT", new MoveCameraCommand(moveDirLeft));
        pInputManager->AddBaseKeyToMap(ControllerButton::ButtonRight, KeyboardButton::D, MouseButton::RMB, ButtonPressType::BUTTON_RELEASED, "MOVE_RIGHT", new MoveCameraCommand(moveDirRight));
        pInputManager->AddBaseKeyToMap(ControllerButton::ButtonLThumbStick, KeyboardButton::NoAction, MouseButton::RMB, ButtonPressType::BUTTON_HOLD, "ROTATE", new RotateCameraCommand());
    }
}
void Vanir::PostInitialize()
{
    InspectorUI* pInspectorUI{ new InspectorUI() };
    AddUI(pInspectorUI);

    ConsoleUI* pConsoleUI{ new ConsoleUI() };
    AddUI(pConsoleUI);

    ViewportUI* pViewportUI{ new ViewportUI() };
    AddUI(pViewportUI);

    HierarchyUI* pHierarchyUI{ new HierarchyUI() };
    AddUI(pHierarchyUI);

    CameraViewportUI* pCameraViewportUI{ new CameraViewportUI() };
    AddUI(pCameraViewportUI);

    ContentBrowserUI* pContentBrowserUI{ new ContentBrowserUI() };
    AddUI(pContentBrowserUI);

    m_pTitlebarUI = new TitlebarUI();
    AddUI(m_pTitlebarUI);

    m_pMenubarUI = new MenubarUI();
    AddUI(m_pMenubarUI);

    InitializeUIs();

    // Create editor commands
    CreateNewSceneCommand* pCreateNewSceneCommand = new CreateNewSceneCommand(pHierarchyUI, pInspectorUI);
    SaveSceneCommand* pSaveSceneCommand = new SaveSceneCommand();
    OpenSceneCommand* pOpenSceneCommand = new OpenSceneCommand();
    m_pMenubarUI->SetCNSC(pCreateNewSceneCommand);

    // Create editor shortcuts
    InputManager* pInputManager{ Locator::GetInputManagerService() };
    pInputManager->AddBaseKeyComboToMap({ KeyboardButton::L_CTRL, KeyboardButton::N }, "", pCreateNewSceneCommand);
    pInputManager->AddBaseKeyComboToMap({ KeyboardButton::L_CTRL, KeyboardButton::S }, "", pSaveSceneCommand);
    pInputManager->AddBaseKeyComboToMap({ KeyboardButton::L_CTRL, KeyboardButton::O }, "", pOpenSceneCommand);
}

void Vanir::Update()
{
    const size_t windowAmount{ m_pUIs.size() };
    for (size_t i{}; i < windowAmount; ++i)
        m_pUIs[i]->Update();
}
void Vanir::FixedUpdate()
{
    const size_t windowAmount{ m_pUIs.size() };
    for (size_t i{}; i < windowAmount; ++i)
        m_pUIs[i]->FixedUpdate();
}

void Vanir::Prerender()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    OpenDockSpace();
}
bool Vanir::PreSideCamRender()
{
    GameObject* pHighlightedGameObject{ GetUI<InspectorUI>()->GetHighlightedCameraGameobject() };
    if (pHighlightedGameObject == nullptr)
        return false;

    CameraComponent* pCameraComponent{ pHighlightedGameObject->GetComponent<CameraComponent>() };
    if (pCameraComponent == nullptr)
        return false;

    Locator::ProvideRenderCameraService(pCameraComponent);

    return true;
}
void Vanir::Render()
{
    for (UI* pUI : m_pUIs)
        pUI->ShowWindow();
}
void Vanir::Postrender()
{
    CloseDockSpace();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void Vanir::InitializeImGui(Window* pWindow, Graphics* pGraphics)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    io.ConfigViewportsNoDecoration = true;

    if (!ImGui_ImplWin32_Init(pWindow->GetWindowHandle()))
        LOG(ErrorLevel::LOG_ERROR, "UIManager::UIManager >> ImGui_ImplWin32_Init << failed");

    if (!ImGui_ImplDX11_Init(pGraphics->GetDevice(), pGraphics->GetDeviceContext()))
        LOG(ErrorLevel::LOG_ERROR, "UIManager::UIManager >> ImGui_ImplDX11_Init << failed");

    // Setup Dear ImGui style
    io.Fonts->AddFontFromFileTTF("../VanaheimEngine/Resources/Fonts/Roboto/Roboto-Bold.ttf", 15.f);
    io.FontDefault = io.Fonts->AddFontFromFileTTF("../VanaheimEngine/Resources/Fonts/Roboto/Roboto-Regular.ttf", 15.f);
    ImGui::StyleColorsDark();
    SetThemeColors();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    m_pMainViewport = ImGui::GetMainViewport();
}
void Vanir::InitializeUIs()
{
    for (UI* pUI : m_pUIs)
        pUI->Initialize(*this);
}
void Vanir::ShutdownImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Vanir::OpenDockSpace()
{
    bool useCustomTitleBar = true;

    InitDockSpace(useCustomTitleBar);

    if (useCustomTitleBar)
        m_pMenubarUI->Render(useCustomTitleBar);
}
void Vanir::InitDockSpace(const bool drawCustomTitleBar)
{
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGui::SetNextWindowPos(m_pMainViewport->WorkPos);
    ImGui::SetNextWindowSize(m_pMainViewport->WorkSize);
    ImGui::SetNextWindowViewport(m_pMainViewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags window_flags{};
    window_flags |= ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    window_flags |= ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    if (!(drawCustomTitleBar) && true)
        window_flags |= ImGuiWindowFlags_MenuBar;

    const bool isMaximized = true;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
            ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
            ImGui::Begin("DockSpace", nullptr, window_flags);
            ImGui::PopStyleColor(); // MenuBarBg
        ImGui::PopStyleVar(2);
    ImGui::PopStyleVar(2);

    if (drawCustomTitleBar)
    {
        float titleBarHeight{};
        m_pTitlebarUI->Render(drawCustomTitleBar, titleBarHeight, m_pMenubarUI);
        ImGui::SetCursorPosY(titleBarHeight);
    }
    
    // Submit the DockSpace
    ImGuiStyle& style = ImGui::GetStyle();
    const float minWinSizeX{ style.WindowMinSize.x };
    style.WindowMinSize.x = 370.f;

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
    }
    style.WindowMinSize.x = minWinSizeX;
}
void Vanir::CloseDockSpace()
{
    ImGui::End();
}

void Vanir::SetThemeColors()
{
    const float alpha{ 1.f };

    ImGuiStyle& style{ ImGui::GetStyle() };
    ImVec4(&colors)[55] { style.Colors };
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, alpha };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.20f, 0.2050f, 0.210f, alpha };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.30f, 0.3050f, 0.310f, alpha };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, alpha };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.20f, 0.2050f, 0.210f, alpha };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.30f, 0.3050f, 0.310f, alpha };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, alpha };

    // Frame bg
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.20f, 0.2050f, 0.210f, alpha };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.30f, 0.3050f, 0.310f, alpha };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, alpha };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, alpha };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, alpha };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, alpha };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, alpha };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.20f, 0.2050f, 0.210f, alpha };

    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, alpha };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, alpha };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1501f, 0.151f, alpha };
}