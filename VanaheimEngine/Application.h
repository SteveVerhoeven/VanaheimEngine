#pragma once
#include <string>

class Application
{
	public:
		Application();
		Application(const std::string& name);
		virtual ~Application() = default;

		Application(const Application& other) = delete;
		Application(Application&& other) = delete;
		Application& operator=(const Application& other) = delete;
		Application& operator=(Application&& other) = delete;

		virtual void Initialize() = 0;
		virtual void PostInitialize() = 0;

		virtual void Update() = 0;
		virtual void FixedUpdate() = 0;

		virtual void Prerender() = 0;
		virtual bool PreSideCamRender() = 0;
		virtual void Render() = 0;
		virtual void Postrender() = 0;

	protected:
	
	private:
		std::string m_Name;
};