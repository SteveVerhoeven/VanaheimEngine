#include "VanaheimPCH.h"
#include "WindowsUtils.h"

#include <string>
#include <commdlg.h>
#include "Window.h"


std::string WindowsUtils::FileDialogs::OpenFile(const char* filter)
{
	CHAR szFile[260]{ 0 };

	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = Locator::GetWindowService()->GetWindowHandle();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetOpenFileNameA(&ofn) == TRUE)
		return ofn.lpstrFile;

	return std::string();
}

std::string WindowsUtils::FileDialogs::SaveFile(const char* filter)
{
	CHAR szFile[260]{ 0 };

	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = Locator::GetWindowService()->GetWindowHandle();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetSaveFileNameA(&ofn) == TRUE)
		return ofn.lpstrFile;

	return std::string();
}
