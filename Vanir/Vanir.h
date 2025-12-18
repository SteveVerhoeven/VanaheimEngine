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

		void SetFullscreen(const bool isFullscreen) { m_EngineFullScreen = isFullscreen; };
		bool GetFullscreen() const { return m_EngineFullScreen; };

	protected:

	private:
		bool m_EngineFullScreen;
		TitlebarUI* m_pTitlebarUI;
		MenubarUI* m_pMenubarUI;
		Window* m_pWindow;
		ImGuiViewport* m_pMainViewport;
		std::vector<UI*> m_pUIs;

		void InitializeImGui(Window* pWindow, Graphics* pGraphics);
		void InitializeUIs();
		void ShutdownImGui();

		// Docking
		void OpenDockSpace();
		void InitDockSpace(const bool drawCustomTitleBar);

		void CloseDockSpace();

		//Style
		void SetThemeColors();
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