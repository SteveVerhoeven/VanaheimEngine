// >----------------------------------< //
// > Copyright 2022 - Vanaheim Game   < //
// > Author: Steve Verhoeven		  < //
// >----------------------------------< //
#include "VanirPCH.h"

// Visual Leak Detector
#if defined(DEBUG) | defined(_DEBUG)
#include <vld.h>
#endif

// Project includes
#include "VanaheimEngine.h"
#include "Vanir.h"
#include "VGame.h"

// Application Entry Point - References:
// - https://docs.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point
// - https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-winmain
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{	
	VanaheimEngine vEngine{};
	vEngine.Initialize(hInstance);

	Vanir vanir{ "Vanir the Vaneheim Editor" };
	vanir.Initialize();

	VGame game{ "Totally awesome game" };
	game.Initialize();

	game.PostInitialize();
	vanir.PostInitialize();
	
	Locator::ProvideEditorService(&vanir);

	return vEngine.EngineLoop(vanir);
}