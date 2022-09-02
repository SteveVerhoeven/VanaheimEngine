#pragma once
#include "UI.h"
#include <filesystem> // C++17

struct DirectoryNode
{
	std::string FullPath;
	std::string FileName;
	std::vector<DirectoryNode> Children;
	bool IsDirectory;
	bool IsOpen;

	bool hasDirectoriesInChildren() const
	{
		for(const DirectoryNode& dn : Children)
			if(dn.IsDirectory)
				return true;

		return false;
	}

	DirectoryNode* FindNodeByFileName(const std::string& fileName, std::vector<DirectoryNode*>& dns)
	{
		DirectoryNode* node{nullptr};

		if(fileName != FileName)
		{
			size_t nbrOfChildren{ Children.size() };
			for(size_t i{}; i < nbrOfChildren; ++i)
			{
				node = Children[i].FindNodeByFileName(fileName, dns);
				if(node != nullptr)
				{
					dns.push_back(&Children[i]);
					dns.push_back(this);
					break;
				}
			}
		}
		else
			node = this;

		return node;
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
		DirectoryNode m_RootNode;

		void Draw();
		void DrawBackButton();
		void DrawFolderHierarchy();
		void DrawFoldersAndFiles(const float thumbnailSize, const float padding);
		void DrawFoldersAndFilesRecursively(const std::filesystem::directory_entry& dirEntry, const ImGuiTreeNodeFlags& flags);
		void DrawSliders(float& padding, float& thumbnailSize);

		void DrawFoldersAndFilesOLD(const float thumbnailSize);

		void AdjustColumnAmount(const float padding, const float thumbnailSize);
		Texture* ChooseWhichIcon(const std::filesystem::directory_entry& directoryEntry);
		void DragDrop(const std::filesystem::path& relativePath);

		// Tree
		void RecursivelyDisplayDirectoryNode(DirectoryNode& parentNode);
		void RecursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator);
		void RecursivelyRecheckDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator);
		DirectoryNode CreateDirectryNodeTreeFromPath(const std::filesystem::path& rootPath);
};