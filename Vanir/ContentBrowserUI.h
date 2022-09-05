#pragma once
#include "UI.h"
#include <filesystem> // C++17

enum State_Flags
{
	IDLE = 0,
	DELETE_NODE = 1 << 0,
	IS_DIRECTORY = 1 << 1,
	IS_OPEN = 1 << 2,
};
DEFINE_ENUM_FLAG_OPERATORS(State_Flags)
struct DirectoryNode
{
	State_Flags StateFlags;
	std::string FullPath;
	std::string FileName;
	std::vector<DirectoryNode> Children;
	
	DirectoryNode* FindNodeByFileName(const std::string& fileName, std::vector<DirectoryNode*>& pDirectoryNodesToChange)
	{
		DirectoryNode* node{nullptr};

		if(fileName != FileName)
		{
			size_t nbrOfChildren{ Children.size() };
			for(size_t i{}; i < nbrOfChildren; ++i)
			{
				node = Children[i].FindNodeByFileName(fileName, pDirectoryNodesToChange);
				if(node != nullptr)
				{
					pDirectoryNodesToChange.push_back(&Children[i]);
					pDirectoryNodesToChange.push_back(this);
					break;
				}
			}
		}
		else
			node = this;

		return node;
	}
	
	// Flags
	void AddFlags(const State_Flags& flags)
	{
		StateFlags |= flags;
	}
	bool HasFlag(const State_Flags& flag) const
	{
		return (StateFlags & flag);
	}
	void RemoveFlag(const State_Flags& flag)
	{
		StateFlags &= ~flag;
	}

	bool hasDirectoriesInChildren() const
	{
		for(const DirectoryNode& dn : Children)
			if(dn.HasFlag(State_Flags::IS_DIRECTORY))
				return true;

		return false;
	}

	// DeleteNode
	void UpdateDirectoryNodesMarkedDelete()
	{
		std::vector<size_t> indices{};

		const size_t nbrOfChildren{ Children.size() };
		for(size_t i{}; i < Children.size(); ++i)
		{
			if(Children[i].HasFlag(State_Flags::DELETE_NODE))
				indices.emplace_back(i);
			else
				Children[i].UpdateDirectoryNodesMarkedDelete();
		}
		
		const size_t nbrOfIndices{ indices.size() };
		for(size_t j{}; j < nbrOfIndices; ++j)
			Children.erase(Children.begin() + indices[j]);
	}
};

class Texture;
class ContentBrowserUI final : public UI
{
	public:
		ContentBrowserUI();
		~ContentBrowserUI();

		ContentBrowserUI(const ContentBrowserUI&) = delete;
		ContentBrowserUI& operator=(const ContentBrowserUI&) = delete;
		ContentBrowserUI(ContentBrowserUI&&) = delete;
		ContentBrowserUI& operator=(ContentBrowserUI&&) = delete;

		void Initialize(const Vanir& vEditor) override;
		void Update() override;
		void FixedUpdate() override;
		void ShowWindow() override;

	protected:

	private:		
		std::filesystem::path m_CurrentDirectory;
		Texture* m_pDirectoryIcon;
		Texture* m_pFileIcon;
		Texture* m_pSceneIcon;
		Texture* m_pTextureIcon;
		Texture* m_pMeshIcon;
		DirectoryNode m_RootNode;

		void Draw();
		void DrawBackButton();
		void DrawFolderHierarchy();
		void DrawFoldersAndFiles(const float rowHeight);
		void DrawFoldersAndFilesRecursively(const std::filesystem::directory_entry& dirEntry, const ImGuiTreeNodeFlags& flags);
		void DrawSliders(float& padding, float& thumbnailSize, const float rowHeight);

		Texture* ChooseWhichIcon(const std::filesystem::directory_entry& directoryEntry);
		void DragDrop(const std::filesystem::path& relativePath);

		// Tree
		DirectoryNode CreateDirectryNodeTreeFromPath(const std::filesystem::path& rootPath);
		void RecursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator);
		void AddDirectory(DirectoryNode& parentNode, const std::filesystem::directory_entry& entry);
		void RecursivelyDisplayDirectoryNode(DirectoryNode& parentNode);
};