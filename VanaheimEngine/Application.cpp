#include "VanaheimPCH.h"
#include "Application.h"

Application::Application()
			: Application("Vanaheim Application")
{}
Application::Application(const std::string& name)
			: m_Name(name)
{}