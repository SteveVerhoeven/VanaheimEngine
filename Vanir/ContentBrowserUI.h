#pragma once
#include "UI.h"
#include <filesystem> // C++17

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

		void Draw();
		void DrawBackButton();
		void DrawFolderHierarchy();
		void DrawFoldersAndFiles(const float thumbnailSize);
		void DrawSliders(float& padding, float& thumbnailSize);

		void AdjustColumnAmount(const float padding, const float thumbnailSize);
		Texture* ChooseWhichIcon(const std::filesystem::directory_entry& directoryEntry);
		void DragDrop(const std::filesystem::path& relativePath);
};