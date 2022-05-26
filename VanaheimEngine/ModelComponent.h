#pragma once
#include "Component.h"
#include <string>

class Mesh;
class Scene;
class Texture;
class Material;
class ModelComponent final : public Component
{
	public:
		ModelComponent();
		ModelComponent(Mesh* pMesh);
		virtual ~ModelComponent() = default;

		ModelComponent(const ModelComponent&) = delete;
		ModelComponent(ModelComponent&&) noexcept = delete;
		ModelComponent& operator=(const ModelComponent&) = delete;
		ModelComponent& operator=(ModelComponent&&) noexcept = delete;

		void Render();
		
		void SetMaterial(Material* pMaterial);
		void SetMesh(Mesh* pMesh) { m_pMesh = pMesh; };

		Mesh* GetMesh() const { return m_pMesh; }
		Material* GetMaterial() const { return m_pMaterial; }
		const std::string& GetFilePath() const;

	protected:
		virtual void Initialize(Scene* pParentScene) override;
		virtual void PostInitialize(Scene* pParentScene) override;
		virtual void Update(const float elapsedSec) override;
		virtual void FixedUpdate(const float timeEachUpdate) override;

	private:
		Mesh* m_pMesh;
		Material* m_pMaterial;
		std::vector<Texture*> m_pTextures;
};