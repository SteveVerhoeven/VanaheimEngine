#include "VanaheimPCH.h"
#include "ResourceManager.h"

#include "Mesh.h"
#include "Line.h"
#include "Material.h"
#include "Texture.h"
#include "Graphics.h"
#include <algorithm>

ResourceManager::ResourceManager()
				: m_p3DMeshes(std::vector<MeshData*>())
				, m_p3DMaterials(std::vector<MaterialData*>())
				, m_pTextures(std::vector<TextureData*>())
				, m_MeshFreeId()
				, m_MaterialFreeId()
				, m_TextureFreeId()
{}
ResourceManager::~ResourceManager()
{
	for (MeshData* pMeshData : m_p3DMeshes)
		DELETE_POINTER(pMeshData);
	for (MaterialData* pMaterialData : m_p3DMaterials)
		DELETE_POINTER(pMaterialData);
	for (TextureData* pTextureData : m_pTextures)
		DELETE_POINTER(pTextureData);

	m_p3DMeshes.clear();
	m_p3DMaterials.clear();
	m_pTextures.clear();
}

void ResourceManager::Initialize()
{}

Mesh* ResourceManager::LoadMesh(const std::string& path)
{
	const std::string fileName{ GetFileNameFromFilePath(path) };
	
	size_t index{};
	if (MeshExists(fileName, index))
	{
		Mesh* pMesh{ dynamic_cast<Mesh*>(m_p3DMeshes[index]->pMesh) };

		// Check if the instance counter is on 0 if so set to 1 to start
		if (pMesh->GetAmountInstances() <= 0)
			pMesh->IncrementInstanceCount();

		pMesh->IncrementInstanceCount();
		return pMesh;
	}

	// If it does not exist
	Mesh* pMesh{ new Mesh(path) };
	pMesh->Initialize();

	// Store the mesh
	MeshData* pMeshData{ new MeshData() };
	pMeshData->ID = GetFreeMeshID();
	pMeshData->name = fileName;
	pMeshData->pMesh = pMesh;
	m_p3DMeshes.push_back(pMeshData);

	// Return the pointer to the mesh
	return pMesh;
}
Texture* ResourceManager::LoadTexture(const std::string& path)
{
	const std::string fileName{ GetFileNameFromFilePath(path) };

	size_t index{};
	if (TextureExists(fileName, index))
	{
		return m_pTextures[index]->pTexture;
	}

	// If it does not exist
	Texture* pTexture{ new Texture(path) };

	// Store the texture
	TextureData* pTextureData{ new TextureData() };
	pTextureData->ID = GetFreeTextureID();
	pTextureData->name = fileName;
	pTextureData->pTexture = pTexture;
	m_pTextures.push_back(pTextureData);

	// Return the pointer to the texture
	return pTexture;
}

void ResourceManager::Store3DMesh(Mesh* pMesh, const std::string& name)
{
	// Store the mesh
	MeshData* pMeshData{ new MeshData() };
	pMeshData->ID = GetFreeMeshID();
	pMeshData->name = name;
	pMeshData->pMesh = pMesh;
	m_p3DMeshes.push_back(pMeshData);
}
void ResourceManager::ResetInstancedMeshes()
{
	for (MeshData* pMeshData : m_p3DMeshes)
		pMeshData->pMesh->SetIsRendered(false);
}

bool ResourceManager::MeshExists(const std::string& name, size_t& index)
{
	// Check if the map for 3D meshes is empty
	if (m_p3DMeshes.empty())
		return false;

	// Check if you find the name in the map
	auto result{ std::find_if(m_p3DMeshes.begin(), m_p3DMeshes.end(), [&](MeshData* pMeshData)
	{
		if (pMeshData->name == name)
		{
			index = pMeshData->ID;
			return true;
		}
		return false;
	}) };

	// Did you find the mesh already of not
	if (result != m_p3DMeshes.end())
		return true;

	return false;
}
bool ResourceManager::MaterialExists(const std::string& name, size_t& index)
{
	// Check if the map for 3D meshes is empty
	if (m_p3DMaterials.empty())
		return false;

	// Check if you find the name in the map
	auto result{ std::find_if(m_p3DMaterials.begin(), m_p3DMaterials.end(), [&](MaterialData* pMaterialData)
	{
		if (pMaterialData->name == name)
		{
			index = pMaterialData->ID;
			return true;
		}
		return false;
	}) };

	// Did you find the mesh already of not
	if (result != m_p3DMaterials.end())
		return true;

	return false;
}
bool ResourceManager::TextureExists(const std::string& name, size_t& index)
{
	// Check if the map for 3D meshes is empty
	if (m_pTextures.empty())
		return false;

	// Check if you find the name in the map
	auto result{ std::find_if(m_pTextures.begin(), m_pTextures.end(), [&](TextureData* pTextureData)
	{
		if (pTextureData->name == name)
		{
			index = pTextureData->ID;
			return true;
		}
		return false;
	}) };

	// Did you find the mesh already of not
	if (result != m_pTextures.end())
		return true;

	return false;
}

const std::string ResourceManager::GetFileNameFromFilePath(const std::string& filePath)
{
	// Remove path
	const size_t lastSlashPosition{ filePath.find_last_of("/\\") };
	std::string completeFileName = filePath.substr(lastSlashPosition + 1);

	// Remove extension
	const size_t lastDotPosition{ completeFileName.find_last_of('.') };
	std::string fileName = completeFileName.substr(0, lastDotPosition);

	return fileName;
}

//bool ResourceManager::MeshAlreadyLoaded(Mesh_Base* pMeshBase, int& meshID)
//{
//	// Check if the map for 3D meshes is empty
//	if (m_p3DMeshes.empty())
//		return false;
//
//	// Check if you find the name in the map
//	auto result{ std::find_if(m_p3DMeshes.begin(), m_p3DMeshes.end(), [&](MeshData* pMeshData)
//	{
//		Line* pLine{ dynamic_cast<Line*>(pMeshData->pMesh) };
//		Line* pLineTest{ dynamic_cast<Line*>(pMeshBase) };
//		Mesh* pMesh{ dynamic_cast<Mesh*>(pMeshData->pMesh) };
//		Mesh* pMeshTest{ dynamic_cast<Mesh*>(pMeshBase) };
//
//		if (pLine && pLine->GetLineType() == pLineTest->GetLineType())
//		{
//			meshID = pMeshData->ID;
//			return true;
//		}
//		if (pMesh && pMesh == pMeshTest)
//		{
//			meshID = pMeshData->ID;
//			return true;
//		}
//		return false;
//	}) };
//
//	// Did you find the mesh already of not
//	if (result != m_p3DMeshes.end())
//		return true;
//
//	return false;
//}