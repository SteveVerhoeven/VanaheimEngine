#include "VanirPCH.h"
#include "ContentBrowserUI.h"

static const std::filesystem::path s_GameAssetPath{ "../Game/Assets" };

ContentBrowserUI::ContentBrowserUI()
	: UI("ContentBrowser")
	, m_UIChanged(true)
	, m_CurrentDirectory(s_GameAssetPath)
	, m_pDirectoryIcon(nullptr)
	, m_pFileIcon(nullptr)
	, m_pSceneIcon(nullptr)
	, m_pMeshIcon(nullptr)
{
	ResourceManager* pResourceManager = Locator::GetResourceManagerService();

	// PERFORMANCE: Load textures ONCE. Reusing these pointers in the draw loop 
	// prevents the GPU from stalling and the CPU from re-loading data from disk.
	m_pDirectoryIcon = pResourceManager->LoadTexture("Resources/Icon/DirectoryIcon.png", true);
	m_pFileIcon = pResourceManager->LoadTexture("Resources/Icon/FileIcon.png", true);
	m_pSceneIcon = pResourceManager->LoadTexture("Resources/Icon/VanaheimSceneIcon.png", true);
	m_pMeshIcon = pResourceManager->LoadTexture("Resources/Icon/MeshIcon.png", true);
}

void ContentBrowserUI::Initialize(const Vanir& /*vEditor*/)
{
	// Build the initial tree and fill the file cache
	m_RootNode = CreateDirectryNodeTreeFromPath(s_GameAssetPath);
	RefreshCache();
}

void ContentBrowserUI::Update()
{
}

void ContentBrowserUI::FixedUpdate()
{
	// PERFORMANCE: Only hit the disk and rebuild the tree when a change is flagged.
	// This removes the heavy I/O overhead from your main loop.
	if (m_UIChanged)
	{
		m_RootNode = CreateDirectryNodeTreeFromPath(s_GameAssetPath);
		RefreshCache();
		m_UIChanged = false;
	}

	m_RootNode.UpdateDirectoryNodesMarkedDelete();
}

void ContentBrowserUI::ShowWindow()
{
	if (!m_RenderUI)
		return;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
	BeginWindowBase(window_flags);
	Draw();
	EndWindowBase();
}

void ContentBrowserUI::Draw()
{
	ImGuiWindow* pWindow = ImGui::FindWindowByName(m_Name.c_str());
	if (!pWindow) return;

	const ImVec2 windowSize = pWindow->Size;

	ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV;
	if (ImGui::BeginTable("Main", 2, flags))
	{
		ImGui::TableSetupColumn("Folder tree", ImGuiTableColumnFlags_NoClip, windowSize.x * 0.2f);
		ImGui::TableSetupColumn("Folder and files", ImGuiTableColumnFlags_NoClip, windowSize.x * 0.8f);
		ImGui::TableHeadersRow();

		float rowHeight = windowSize.y - (ImGui::TableGetHeaderRowHeight() * 3);

		ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
		ImGui::TableNextColumn();

		// Draw the Folder hierarchy on the left
		DrawFolderHierarchy();

		// Next column
		ImGui::TableNextColumn();

		// Draw the Folder and Files on the right
		DrawFoldersAndFiles(rowHeight);

		ImGui::EndTable();
	}

	// Right click on empty space
	if (ImGui::BeginPopupContextWindow(0, 1, false))
	{
		if (ImGui::MenuItem("Create folder"))
		{
			std::filesystem::path currentPath = m_CurrentDirectory / "New Folder";
			if (std::filesystem::create_directory(currentPath))
			{
				m_UIChanged = true;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ContentBrowserUI::DrawBackButton()
{
	if (m_CurrentDirectory != s_GameAssetPath)
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
			m_UIChanged = true;
		}
	}
}

void ContentBrowserUI::RefreshCache()
{
	m_DirectoryCache.clear();
	if (!std::filesystem::exists(m_CurrentDirectory)) return;

	// PERFORMANCE: Convert directory entries to a flat list once.
	// This avoids calling path() and filename() every single frame in Draw().
	for (const auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		CachedFile file;
		file.Path = entry.path();
		file.FileName = file.Path.filename().string();
		file.IsDirectory = entry.is_directory();
		file.Icon = ChooseWhichIcon(entry);

		m_DirectoryCache.push_back(file);
	}
}

Texture* ContentBrowserUI::ChooseWhichIcon(const std::filesystem::directory_entry& entry)
{
	if (entry.is_directory()) return m_pDirectoryIcon;

	auto ext = entry.path().extension();
	if (ext == ".Vanaheim") return m_pSceneIcon;
	if (ext == ".fbx" || ext == ".obj") return m_pMeshIcon;

	// Default file icon
	return m_pFileIcon;
}

void ContentBrowserUI::DrawFoldersAndFiles(const float rowHeight)
{
	static float padding = 16.f;
	static float thumbnailSize = 48.f;

	float cellSize = thumbnailSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1) columnCount = 1;

	if (ImGui::BeginTable("FoldersAndFiles", columnCount, ImGuiTableFlags_NoBordersInBody))
	{
		for (const auto& file : m_DirectoryCache)
		{
			ImGui::TableNextColumn();
			ImGui::PushID(file.FileName.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0,0,0,0 });
			ImGui::ImageButton((ImTextureID)file.Icon->GetShaderResourceView(), { thumbnailSize, thumbnailSize });
			ImGui::PopStyleColor();

			// Drag & Drop
			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relPath = std::filesystem::relative(file.Path, s_GameAssetPath);
				const wchar_t* itemPath = relPath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			// Context Menu
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete"))
				{
					std::filesystem::remove_all(file.Path);
					m_UIChanged = true;
				}
				ImGui::EndPopup();
			}

			// Navigation
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (file.IsDirectory)
				{
					m_CurrentDirectory = file.Path;
					m_UIChanged = true;
				}
			}

			ImGui::TextWrapped("%s", file.FileName.c_str());
			ImGui::PopID();
		}
		ImGui::EndTable();
	}

	DrawSliders(padding, thumbnailSize, rowHeight);
}

void ContentBrowserUI::DrawFolderHierarchy()
{
	ImGui::PushID(&m_RootNode);
	if (m_RootNode.HasFlag(State_Flags::IS_DIRECTORY))
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		RecursivelyDisplayDirectoryNode(m_RootNode);
	}
	ImGui::PopID();
}

void ContentBrowserUI::RecursivelyDisplayDirectoryNode(DirectoryNode& parentNode)
{
	ImGui::PushID(&parentNode);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (!parentNode.hasDirectoriesInChildren())
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (parentNode.HasFlag(State_Flags::IS_OPEN))
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);

	bool opened = ImGui::TreeNodeEx(parentNode.FileName.c_str(), flags);

	if (ImGui::IsItemClicked())
	{
		m_CurrentDirectory = parentNode.FullPath;
		m_UIChanged = true;

		if (opened) parentNode.AddFlags(State_Flags::IS_OPEN);
		else parentNode.RemoveFlag(State_Flags::IS_OPEN);
	}

	if (opened)
	{
		for (auto& child : parentNode.Children)
			RecursivelyDisplayDirectoryNode(child);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void ContentBrowserUI::DrawSliders(float& padding, float& thumbnailSize, const float rowHeight)
{
	ImGui::SetCursorPosY(rowHeight);
	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 48, 70);
	ImGui::SliderFloat("Padding", &padding, 16, 32);
}

DirectoryNode ContentBrowserUI::CreateDirectryNodeTreeFromPath(const std::filesystem::path& rootPath)
{
	DirectoryNode rootNode{};
	rootNode.FullPath = rootPath.string();
	rootNode.FileName = rootPath.filename().string();
	if (std::filesystem::is_directory(rootPath))
	{
		rootNode.AddFlags(State_Flags::IS_DIRECTORY);
		RecursivelyAddDirectoryNodes(rootNode, std::filesystem::directory_iterator(rootPath));
	}
	return rootNode;
}

void ContentBrowserUI::RecursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator)
{
	for (const auto& entry : directoryIterator)
	{
		if (!entry.is_directory()) continue;

		DirectoryNode& childNode = parentNode.Children.emplace_back();
		childNode.FullPath = entry.path().string();
		childNode.FileName = entry.path().filename().string();
		childNode.AddFlags(State_Flags::IS_DIRECTORY);

		RecursivelyAddDirectoryNodes(childNode, std::filesystem::directory_iterator(entry.path()));
	}

	std::sort(parentNode.Children.begin(), parentNode.Children.end(), [](const DirectoryNode& a, const DirectoryNode& b) {
		return a.FileName < b.FileName;
		});
}