#include "VanirPCH.h"
#include "ContentBrowserUI.h"

static const std::filesystem::path s_GameAssetPath{ "../Game/Assets" };
static const std::filesystem::path s_EditorResourcePath{ "Resources" };
static const std::filesystem::path s_GameResourcePath{ "../Game/Resources" };
static const std::filesystem::path s_EngineResourcePath{ "../VanaheimEngine/Resources" };

ContentBrowserUI::ContentBrowserUI()
				 : UI("ContentBrowser", DirectX::XMFLOAT2{ 0.f, 0.f }, DirectX::XMFLOAT2{ 0.f, 0.f })
				 , m_CurrentDirectory(s_GameAssetPath)
{
	ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };

	m_pDirectoryIcon = pResourceManager->LoadTexture("Resources/Icon/DirectoryIcon.png", true);
	m_pFileIcon = pResourceManager->LoadTexture("Resources/Icon/FileIcon.png", true);
	m_pSceneIcon = pResourceManager->LoadTexture("Resources/Icon/VanaheimSceneIcon.png", true);
}
ContentBrowserUI::~ContentBrowserUI()
{}

void ContentBrowserUI::Initialize(const Vanir& /*vEditor*/)
{}
void ContentBrowserUI::Update()
{}
void ContentBrowserUI::FixedUpdate()
{}
void ContentBrowserUI::ShowWindow()
{
	if (!m_RenderUI)
		return;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoDecoration;

	BeginWindowBase(window_flags);
	Draw();
	EndWindowBase();
}

void ContentBrowserUI::Draw()
{
	DrawBackButton();

	// Folder hierarchy on the left
	ImGui::Columns(2, 0, false);
	DrawFolderHierarchy();

	// Folder and files
	ImGui::NextColumn();
	
	static float padding{ 16.f };
	static float thumbnailSize{ 32.f };
	AdjustColumnAmount(padding, thumbnailSize);

	DrawFoldersAndFiles(thumbnailSize);

	ImGui::Columns(1);

	// Bottom sliders
	DrawSliders(padding, thumbnailSize);
}

void ContentBrowserUI::DrawBackButton()
{
	if (m_CurrentDirectory != std::filesystem::path(s_GameAssetPath))
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}
}

void ContentBrowserUI::DrawFolderHierarchy()
{
	/*for(const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		
	}*/
}
void ContentBrowserUI::DrawFoldersAndFiles(const float thumbnailSize)
{
	for(const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		const std::filesystem::path path{ directoryEntry.path() };
		const std::filesystem::path relativePath{ std::filesystem::relative(path, s_GameAssetPath) };

		std::string filenameString{ relativePath.filename().string() };

		ImGui::PushID(filenameString.c_str());
		Texture* icon{ ChooseWhichIcon(directoryEntry) };
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0,0,0,0 });
		ImGui::ImageButton(icon->GetShaderResourceView(), { thumbnailSize, thumbnailSize });
		
		DragDrop(relativePath);
		
		ImGui::PopStyleColor();
		if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if(directoryEntry.is_directory())
			{
				m_CurrentDirectory /= path.filename();
			}
		}
		ImGui::TextWrapped(filenameString.c_str());

		ImGui::NextColumn();

		ImGui::PopID();
	}
}

void ContentBrowserUI::DrawSliders(float& padding, float& thumbnailSize)
{
	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
	ImGui::SliderFloat("Padding", &padding, 0, 32);
}
void ContentBrowserUI::AdjustColumnAmount(const float padding, const float thumbnailSize)
{
	float cellSize{ thumbnailSize + padding };

	const float panelWidth{ ImGui::GetContentRegionAvail().x };
	int columnCount{ (int)(panelWidth / cellSize) };
	if (columnCount < 1)
		columnCount = 1;
	ImGui::Columns(columnCount, 0, false);
}
Texture* ContentBrowserUI::ChooseWhichIcon(const std::filesystem::directory_entry& directoryEntry)
{
	Texture* icon{ nullptr };
	// If it is a directory
	if (directoryEntry.is_directory())
		icon = m_pDirectoryIcon;
	// If it is a Vanaheimscene
	else if (directoryEntry.path().extension() == ".Vanaheim")
		icon = m_pSceneIcon;
	/*
	// If it is a 3D Model
	else if(directoryEntry.path().extension() == ".Vanaheim")
		icon = m_pSceneIcon;
	// If it is a Texture
	else if(directoryEntry.path().extension() == ".Vanaheim")
		icon = m_pSceneIcon;
	*/
	// If it is not a recognized file type
	else
		icon = m_pFileIcon;

	return icon;
}
void ContentBrowserUI::DragDrop(const std::filesystem::path& relativePath)
{
	if(ImGui::BeginDragDropSource())
	{
		const wchar_t* itemPath{ relativePath.c_str() };
		ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
		ImGui::EndDragDropSource();
	}
}