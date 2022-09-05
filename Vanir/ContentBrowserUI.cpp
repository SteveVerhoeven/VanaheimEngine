#include "VanirPCH.h"
#include "ContentBrowserUI.h"

static const std::filesystem::path s_GameAssetPath{ "../Game/Assets" };
static const std::filesystem::path s_EditorResourcePath{ "Resources" };
static const std::filesystem::path s_GameResourcePath{ "../Game/Resources" };
static const std::filesystem::path s_EngineResourcePath{ "../VanaheimEngine/Resources" };

ContentBrowserUI::ContentBrowserUI()
				 : UI("ContentBrowser")
				 , m_CurrentDirectory(s_GameAssetPath)
				 , m_pDirectoryIcon(nullptr)
				 , m_pFileIcon(nullptr)
				 , m_pTextureIcon(nullptr)
				 , m_pMeshIcon(nullptr)
				 , m_RootNode()
{
	ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };

	m_pDirectoryIcon = pResourceManager->LoadTexture("Resources/Icon/DirectoryIcon.png", true);
	m_pFileIcon = pResourceManager->LoadTexture("Resources/Icon/FileIcon.png", true);
	m_pSceneIcon = pResourceManager->LoadTexture("Resources/Icon/VanaheimSceneIcon.png", true);
	m_pMeshIcon = pResourceManager->LoadTexture("Resources/Icon/MeshIcon.png", true);
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

	m_RootNode.UpdateDirectoryNodesMarkedDelete();
}
void ContentBrowserUI::ShowWindow()
{
	if (!m_RenderUI)
		return;

	ImGuiWindowFlags window_flags{};
	//window_flags |= ImGuiWindowFlags_MenuBar;

	BeginWindowBase(window_flags);
	Draw();
	EndWindowBase();
}

void ContentBrowserUI::Draw()
{
	// Tree view for folders (Left side)
	// Reference: https://github.com/ocornut/imgui/issues/5137

	// Get the window size
	ImGuiWindow* pWindow = ImGui::FindWindowByName(m_Name.c_str());
	const ImVec2 windowSize = pWindow->Size;
	
	// Column logic
	ImGuiTableFlags flags{};
	flags |= ImGuiTableFlags_SizingFixedFit;
	flags |= ImGuiTableFlags_BordersInnerV;
	if (ImGui::BeginTable("Main", 2, flags))
	{
		ImGui::TableSetupColumn("Folder tree", ImGuiTableColumnFlags_NoClip, windowSize.x / 100 * 20);
		ImGui::TableSetupColumn("Folder and files", ImGuiTableColumnFlags_NoClip, windowSize.x / 100 * 80);
		ImGui::TableHeadersRow();

		ImVec2 rowSize{ windowSize };
		//rowSize.y -= pWindow->MenuBarHeight();
		rowSize.y -= ImGui::TableGetHeaderRowHeight() * 3;

		ImGui::TableNextRow(ImGuiTableRowFlags_None, rowSize.y);
		ImGui::TableNextColumn();

		// Draw the Folder hierarchy on the left
		DrawFolderHierarchy();

		// Next column
		ImGui::TableNextColumn();

		// Draw the Folder and Files on the right
		DrawFoldersAndFiles(rowSize.y);

		ImGui::EndTable();
	}	

	// Inputs
	// Right click on empty space in content browser
	if(ImGui::BeginPopupContextWindow(0, 1, false))
	{
		if(ImGui::MenuItem("Create folder"))
		{
			std::filesystem::path currentPath{ m_CurrentDirectory };
			currentPath /= "Folder";

			if(!std::filesystem::create_directory(currentPath, m_CurrentDirectory))
			{
				LOG_INFO("Folder already exists");
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
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
	{
		if(m_RootNode.HasFlag(State_Flags::IS_DIRECTORY))
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
						m_CurrentDirectory = m_RootNode.FullPath;

				// Display the children under this node
				for(DirectoryNode& childNode : m_RootNode.Children)
					RecursivelyDisplayDirectoryNode(childNode);

				ImGui::TreePop();
			}
		}
	}
	ImGui::PopID();
}
void ContentBrowserUI::DrawFoldersAndFiles(const float rowHeight)
{
	// Settings for zooming with sliders
	static float padding{ 16.f };
	static float thumbnailSize{ 48.f };

	// Calculate the column amount
	float cellSize{ thumbnailSize + padding };
	const float panelWidth{ ImGui::GetColumnWidth(1) };
	int columnCount{ (int)(panelWidth / cellSize) };
	if(columnCount < 1)
		columnCount = 1;

	// Table logic
	ImGuiTableFlags flags2{};
	flags2 |= ImGuiTableFlags_Resizable;
	flags2 |= ImGuiTableFlags_NoBordersInBody;
	if(ImGui::BeginTable("Folders and files", columnCount, flags2))
	{
		// Go over all the directory entries
		int columnCounter{ 0 };
		ImGui::TableNextColumn();
		for(const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			bool deleteFolder{ false };
			// 
			const std::filesystem::path path{ directoryEntry.path() };
			const std::filesystem::path relativePath{ std::filesystem::relative(path, s_GameAssetPath) };
			const std::string filenameString{ relativePath.filename().string() };

			ImGui::PushID(filenameString.c_str());
			{
				Texture* pIcon{ ChooseWhichIcon(directoryEntry) };
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0,0,0,0 });
				{
					ImGui::ImageButton(pIcon->GetShaderResourceView(), { thumbnailSize, thumbnailSize });
					DragDrop(relativePath);
				}
				ImGui::PopStyleColor();

				// Input
				// Right click on empty space in hierarchy
				if(ImGui::BeginPopupContextItem())
				{
					if(ImGui::MenuItem("Delete folder"))
					{
						deleteFolder = true;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				// Button is double clicked
				if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if(directoryEntry.is_directory())
					{
						// Adjust the current directory to start drawing the folders and files inside that directory
						m_CurrentDirectory /= path.filename();

						// Find the parents nodes for the folder hierarchy and set them to IsOpen
						std::vector<DirectoryNode*> pDirectoryNodesToChange{};
						m_RootNode.FindNodeByFileName(filenameString, pDirectoryNodesToChange);

						for(DirectoryNode* pDirectoryNode : pDirectoryNodesToChange)
							pDirectoryNode->AddFlags(State_Flags::IS_OPEN);
					}
				}
				ImGui::TextWrapped(filenameString.c_str());

				// Column vs Row logic
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
			}	
			ImGui::PopID();

			if(deleteFolder)
			{
				// Remove directory
				std::filesystem::remove_all(directoryEntry.path());

				// Remove DirectoryNode from tree
				std::vector<DirectoryNode*> pDirectoryNodes{};
				auto node = m_RootNode.FindNodeByFileName(directoryEntry.path().filename().string(), pDirectoryNodes);
				node->AddFlags(State_Flags::DELETE_NODE);
			}
		}
		ImGui::EndTable();
	}

	// Bottom sliders
	DrawSliders(padding, thumbnailSize, rowHeight);
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

void ContentBrowserUI::DrawSliders(float& padding, float& thumbnailSize, const float rowHeight)
{
	// Set the cursor to the bottom for the sliders
	ImGui::SetCursorPosY(rowHeight);

	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 48, 70);
	ImGui::SliderFloat("Padding", &padding, 16, 32);
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

DirectoryNode ContentBrowserUI::CreateDirectryNodeTreeFromPath(const std::filesystem::path& rootPath)
{
	DirectoryNode rootNode{};
	if(m_RootNode.FileName != "")
		rootNode = m_RootNode;

	rootNode.FullPath = rootPath.u8string();
	rootNode.FileName = rootPath.filename().u8string();
	if(std::filesystem::is_directory(rootPath))
	{
		rootNode.AddFlags(State_Flags::IS_DIRECTORY);
		RecursivelyAddDirectoryNodes(rootNode, std::filesystem::directory_iterator(rootPath));
	}

	return rootNode;
}
void ContentBrowserUI::RecursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator)
{
	if(parentNode.Children.empty())
	{
		for(const std::filesystem::directory_entry& entry : directoryIterator)
		{
			AddDirectory(parentNode, entry);
		}

		auto moveDirectoriesToFront = [](const DirectoryNode& a, const DirectoryNode& b) { return (a.HasFlag(State_Flags::IS_DIRECTORY) > b.HasFlag(State_Flags::IS_DIRECTORY)); };
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
				if(entry.is_directory())
				{
					parentNode.Children[index].AddFlags(State_Flags::IS_DIRECTORY);
					RecursivelyAddDirectoryNodes(parentNode.Children[index], std::filesystem::directory_iterator(entry));
				}
			}
			else
			{
				LOG_INFO(entry.path().filename().string() + " Not found! \n");
				AddDirectory(parentNode, entry);
			}			
		}		
	}
}
void ContentBrowserUI::AddDirectory(DirectoryNode& parentNode, const std::filesystem::directory_entry& entry)
{
	DirectoryNode& childNode = parentNode.Children.emplace_back();
	childNode.FullPath = entry.path().u8string();
	childNode.FileName = entry.path().filename().u8string();
	if(entry.is_directory())
	{
		childNode.AddFlags(State_Flags::IS_DIRECTORY);
		RecursivelyAddDirectoryNodes(childNode, std::filesystem::directory_iterator(entry));
	}

	auto moveDirectoriesToFront = [](const DirectoryNode& a, const DirectoryNode& b) { return (a.HasFlag(State_Flags::IS_DIRECTORY) > b.HasFlag(State_Flags::IS_DIRECTORY)); };
	std::sort(parentNode.Children.begin(), parentNode.Children.end(), moveDirectoriesToFront);
}
void ContentBrowserUI::RecursivelyDisplayDirectoryNode(DirectoryNode& parentNode)
{
	ImGui::PushID(&parentNode);
	if(parentNode.HasFlag(State_Flags::IS_DIRECTORY))
	{
		ImGuiTreeNodeFlags flags{};
		// Dont draw an arrow when the folder is empty of folders
		if (!parentNode.hasDirectoriesInChildren())
			flags |= ImGuiTreeNodeFlags_Leaf;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;

		// Check if the node should be open and set the next item accordingly
		if(parentNode.HasFlag(State_Flags::IS_OPEN))
			ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		else
			ImGui::SetNextItemOpen(false, ImGuiCond_Always);

		bool opened{ ImGui::TreeNodeEx(parentNode.FileName.c_str(), flags) };	
		if(opened)
		{
			// Check if the item is clicked to remember the open/close state next draw
			if(ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				if(!opened)
					parentNode.RemoveFlag(State_Flags::IS_OPEN);
				else
					parentNode.AddFlags(State_Flags::IS_OPEN);

				if(parentNode.HasFlag(State_Flags::IS_DIRECTORY))
					m_CurrentDirectory = parentNode.FullPath;
			}

			for(DirectoryNode& childNode : parentNode.Children)
				RecursivelyDisplayDirectoryNode(childNode);

			ImGui::TreePop();
		}
	}
	ImGui::PopID();
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
	// If it is a 3D Model
	else if(directoryEntry.path().extension() == ".obj" || directoryEntry.path().extension() == ".fbx")
		icon = m_pMeshIcon;
	// If it is a Texture
	else if(directoryEntry.path().extension() == ".png" ||
			directoryEntry.path().extension() == ".jpg" ||
			directoryEntry.path().extension() == ".jepg" ||
			directoryEntry.path().extension() == ".bmp")
		icon = Locator::GetResourceManagerService()->LoadTexture(directoryEntry.path().string(), true);
	// If it is not a recognized file type
	else
		icon = m_pFileIcon;

	return icon;
}