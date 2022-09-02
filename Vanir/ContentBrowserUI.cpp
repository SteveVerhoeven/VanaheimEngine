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
{
	m_RootNode = CreateDirectryNodeTreeFromPath(m_CurrentDirectory);
}
void ContentBrowserUI::Update()
{}
void ContentBrowserUI::FixedUpdate()
{
	m_RootNode = CreateDirectryNodeTreeFromPath(s_GameAssetPath);
}
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
	// https://github.com/ocornut/imgui/issues/5137

	// Get the total width of the window
	const float totalWidth{ ImGui::GetContentRegionAvail().x };

	// Create the columns
	ImGui::BeginColumns("Main", 2, ImGuiOldColumnFlags_GrowParentContentsSize);
	ImGui::SetColumnWidth(0, totalWidth / 100 * 20);	// 20% of the total width
	ImGui::SetColumnWidth(1, totalWidth / 100 * 80);	// 80% of the total width

	// Draw the Folder hierarchy on the left
	DrawFolderHierarchy();

	// Next column
	ImGui::NextColumn();

	// Draw the Folder and Files on the right
	static float padding{ 16.f };
	static float thumbnailSize{ 48.f };
	DrawFoldersAndFiles(thumbnailSize, padding);

	// End the column logic
	ImGui::EndColumns();

	// Get some space between sliders and the rest
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

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
	ImGui::PushID(&m_RootNode);
	if(m_RootNode.IsDirectory)
	{
		ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_SpanAvailWidth };
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;

		// Set the first tree node as default open (Assets)
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		const bool opened{ ImGui::TreeNodeEx(m_RootNode.FileName.c_str(), flags) };
		if(opened)
		{
			// Check if the treenode is clicked
			if(ImGui::IsItemClicked(ImGuiMouseButton_Left))
				if(m_RootNode.IsDirectory)
					m_CurrentDirectory = m_RootNode.FullPath;

			// Display the children under this node
			for(DirectoryNode& childNode : m_RootNode.Children)
				RecursivelyDisplayDirectoryNode(childNode);

			ImGui::TreePop();
		}
	}
	ImGui::PopID();
}
void ContentBrowserUI::DrawFoldersAndFiles(const float thumbnailSize, const float padding)
{
	float cellSize{ thumbnailSize + padding };

	const float panelWidth{ ImGui::GetColumnWidth(1) };
	int columnCount{ (int)(panelWidth / cellSize) };
	if(columnCount < 1)
		columnCount = 1;

	ImGuiTableFlags flags2{};
	flags2 |= ImGuiTableFlags_Resizable;
	flags2 |= ImGuiTableFlags_NoBordersInBody;
	if(ImGui::BeginTable("Folders and files", columnCount, flags2))
	{
		int columnCounter{ 0 };
		ImGui::TableNextColumn();
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
					// Adjust the current directory to start drawing the folders and files inside that directory
					m_CurrentDirectory /= path.filename();

					// Find the parents nodes for the folder hierarchy and set them to IsOpen
					{
						std::vector<DirectoryNode*> pDNs{};
						DirectoryNode* node = m_RootNode.FindNodeByFileName(path.filename().string(), pDNs);
						UNREFERENCED_PARAMETER(node);

						for(DirectoryNode* pDN: pDNs)
						{
							pDN->IsOpen = true;
						}
					}
				}
			}
			ImGui::TextWrapped(filenameString.c_str());

			if(columnCounter < columnCount)
			{
				ImGui::TableNextColumn();
				++columnCounter;
			}
			else
			{
				ImGui::TableNextRow();
				columnCounter = 0;
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}
}

void ContentBrowserUI::DrawFoldersAndFilesRecursively(const std::filesystem::directory_entry& dirEntry, const ImGuiTreeNodeFlags& flags)
{
	ImGui::PushID(&dirEntry);
	if(dirEntry.is_directory())
	{
		const std::string fileName{ dirEntry.path().filename().string() };
		bool opened{ ImGui::TreeNodeEx(fileName.c_str(), flags) };
		if(opened)
		{
			for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(dirEntry))
				DrawFoldersAndFilesRecursively(entry, flags);

			ImGui::TreePop();
		}
	}
	ImGui::PopID();
}

void ContentBrowserUI::DrawSliders(float& padding, float& thumbnailSize)
{
	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 48, 512);
	ImGui::SliderFloat("Padding", &padding, 16, 32);
}
void ContentBrowserUI::DrawFoldersAndFilesOLD(const float thumbnailSize)
{
	//
	// imgui_demo.cpp Line 4821
	//

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
void ContentBrowserUI::AdjustColumnAmount(const float padding, const float thumbnailSize)
{
	float cellSize{ thumbnailSize + padding };

	const float panelWidth{ 600.f };
	int columnCount{ (int)(panelWidth / cellSize) };
	if (columnCount < 1)
		columnCount = 1;

	ImGui::BeginColumns("Folders and files", columnCount, ImGuiOldColumnFlags_NoBorder);
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



void ContentBrowserUI::RecursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator)
{
	//parentNode.Children.clear();
	
	if(parentNode.Children.empty())
	{
		for(const std::filesystem::directory_entry& entry : directoryIterator)
		{
			DirectoryNode& childNode = parentNode.Children.emplace_back();
			childNode.FullPath = entry.path().u8string();
			childNode.FileName = entry.path().filename().u8string();
			if(childNode.IsDirectory = entry.is_directory(); childNode.IsDirectory)
				RecursivelyAddDirectoryNodes(childNode, std::filesystem::directory_iterator(entry));
		}

		auto moveDirectoriesToFront = [](const DirectoryNode& a, const DirectoryNode& b) { return (a.IsDirectory > b.IsDirectory); };
		std::sort(parentNode.Children.begin(), parentNode.Children.end(), moveDirectoriesToFront);
	}
	else
	{
		for(const std::filesystem::directory_entry& entry : directoryIterator)
		{
			const std::string dirFileName{ entry.path().filename().string() };
			std::vector<DirectoryNode>::iterator result = std::find_if(parentNode.Children.begin(), parentNode.Children.end(), [&](DirectoryNode& dn)
			{
				if(dn.FileName == dirFileName)
					return true;

				return false;
			});

			if(result != parentNode.Children.end())
			{
				size_t index = std::distance(parentNode.Children.begin(), result);
				parentNode.Children[index].FullPath = entry.path().u8string();
				parentNode.Children[index].FileName = entry.path().filename().u8string();
				parentNode.Children[index].IsDirectory = entry.is_directory();
				if(parentNode.Children[index].IsDirectory)
					RecursivelyAddDirectoryNodes(parentNode.Children[index], std::filesystem::directory_iterator(entry));
			}
			else
			{
				DirectoryNode& childNode = parentNode.Children.emplace_back();
				childNode.FullPath = entry.path().u8string();
				childNode.FileName = entry.path().filename().u8string();
				if(childNode.IsDirectory = entry.is_directory(); childNode.IsDirectory)
					RecursivelyAddDirectoryNodes(childNode, std::filesystem::directory_iterator(entry));
			}
			
		}
		auto moveDirectoriesToFront = [](const DirectoryNode& a, const DirectoryNode& b) { return (a.IsDirectory > b.IsDirectory); };
		std::sort(parentNode.Children.begin(), parentNode.Children.end(), moveDirectoriesToFront);
	}
}
void ContentBrowserUI::RecursivelyRecheckDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator)
{
	for(const std::filesystem::directory_entry& entry : directoryIterator)
	{
		std::vector<DirectoryNode>::iterator result = std::find_if(parentNode.Children.begin(), parentNode.Children.end(), [&](DirectoryNode& dn)
		{
			if(dn.FullPath == entry.path().u8string())
				return true;

			return false;
		});

		if(result != parentNode.Children.end())
		{
			result->FullPath = entry.path().u8string();
			result->FileName = entry.path().filename().u8string();
			result->IsDirectory = entry.is_directory();
			if(result->IsDirectory)
				RecursivelyRecheckDirectoryNodes(*result, std::filesystem::directory_iterator(entry));
		}
		else
		{
			DirectoryNode& childNode = parentNode.Children.emplace_back();
			childNode.FullPath = entry.path().u8string();
			childNode.FileName = entry.path().filename().u8string();
			childNode.IsDirectory = entry.is_directory();
			if(childNode.IsDirectory)
				RecursivelyRecheckDirectoryNodes(childNode, std::filesystem::directory_iterator(entry));
		}
		
		auto moveDirectoriesToFront = [](const DirectoryNode& a, const DirectoryNode& b) { return (a.IsDirectory > b.IsDirectory); };
		std::sort(parentNode.Children.begin(), parentNode.Children.end(), moveDirectoriesToFront);
	}
}
DirectoryNode ContentBrowserUI::CreateDirectryNodeTreeFromPath(const std::filesystem::path& rootPath)
{
	DirectoryNode rootNode{};
	if(m_RootNode.FileName != "")
		rootNode = m_RootNode;

	rootNode.FullPath = rootPath.u8string();
	rootNode.FileName = rootPath.filename().u8string();
	rootNode.IsDirectory = std::filesystem::is_directory(rootPath);
	if(rootNode.IsDirectory)
		RecursivelyAddDirectoryNodes(rootNode, std::filesystem::directory_iterator(rootPath));

	return rootNode;
}
void ContentBrowserUI::RecursivelyDisplayDirectoryNode(DirectoryNode& parentNode)
{
	ImGui::PushID(&parentNode);
	if(parentNode.IsDirectory)
	{
		ImGuiTreeNodeFlags flags{};
		// Dont draw an arrow when the folder is empty of folders
		if (!parentNode.hasDirectoriesInChildren())
			flags |= ImGuiTreeNodeFlags_Leaf;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;

		// Check if the node should be open and set the next item accordingly
		if(parentNode.IsOpen)
			ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		else
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);


		bool opened{ ImGui::TreeNodeEx(parentNode.FileName.c_str(), flags) };	
		// Check if the item is clicked to remember the open/close state next draw
		if(ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			if (!opened)
				parentNode.IsOpen = false;
			else
				parentNode.IsOpen = true;

			if(parentNode.IsDirectory)
				m_CurrentDirectory = parentNode.FullPath;
		}

		if(opened)
		{
			for(DirectoryNode& childNode : parentNode.Children)
				RecursivelyDisplayDirectoryNode(childNode);

			ImGui::TreePop();
		}
	}
	ImGui::PopID();
}

