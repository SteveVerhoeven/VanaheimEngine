#pragma once
#include <Application.h>

// Editor for VanaheimEngine
class Vanir final : public Application
{
	public:
		Vanir(const std::string& name);
		virtual ~Vanir();

		Vanir(const Vanir& other) = delete;
		Vanir(Vanir&& other) = delete;
		Vanir& operator=(const Vanir& other) = delete;
		Vanir& operator=(Vanir&& other) = delete;

		void Initialize() override;
		void PostInitialize() override;
		
		void Update() override;
		void FixedUpdate() override;

		void Prerender() override;
		bool PreSideCamRender() override;
		void Render() override;
		void Postrender() override;

		void AddUI(UI* pUI) { m_pUIs.push_back(pUI); }
		template <class T>
		T* GetUI() const;

	protected:

	private:
		Window* m_pWindow;
		ImGuiViewport* m_pMainViewport;
		std::vector<UI*> m_pUIs;

		void InitializeImGui(Window* pWindow, Graphics* pGraphics);
		void InitializeUIs();
		void ShutdownImGui();

		// Docking
		void OpenDockSpace();
		void InitDockSpace();
		void FileMenu();
		void EditMenu();
		void WindowMenu();
		void ToolMenu();
		void CloseDockSpace();

		//Style
		void SetThemeColors();

		// Scenes
		Scene* CreateNewScene(SceneManager* pSceneManager);
};

template<class T>
inline T* Vanir::GetUI() const
{
	for (UI* pUI : m_pUIs)
	{
		T* pTUI = dynamic_cast<T*>(pUI);
		if (pTUI)
			return pTUI;
	}

	return nullptr;
}