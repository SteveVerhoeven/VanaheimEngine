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




Mesh* ResourceManager::Load3DMesh(const std::string& name, const std::string& path)
{
	int meshID{};
	// Check if the Mesh exists already and return the id if it does
	if (MeshAlreadyParsed(name, meshID))
	{
		Mesh* pMesh{ dynamic_cast<Mesh*>(m_p3DMeshes[meshID]->pMesh) };

		// Check if the instance counter is on 0 if so set to 1 to start
		if (pMesh->GetAmountInstances() <= 0)
			pMesh->IncrementInstanceCount();

		pMesh->IncrementInstanceCount();
		return pMesh;
		//return m_p3DMeshes[meshID]->pMesh;
	}

	// If it does not exist
	Mesh* pMesh{ new Mesh(path) };
	pMesh->Initialize();

	// Store the mesh
	MeshData* pMeshData{ new MeshData() };
	pMeshData->ID = GetFreeMeshID();
	pMeshData->name = name;
	pMeshData->pMesh = pMesh;
	m_p3DMeshes.push_back(pMeshData);

	// Return the pointer to the mesh
	return pMesh;
}
Mesh_Base* ResourceManager::Load3DMesh(Mesh_Base* pMesh, GameObject* pParentGO)
{
	int meshID{};
	// Check if the Mesh exists already and return the id if it does
	if (MeshAlreadyLoaded(pMesh, meshID))
	{
		Line* pLine{ dynamic_cast<Line*>(pMesh) };
		if (pLine)
		{
			Line* pTempLine{ dynamic_cast<Line*>(m_p3DMeshes[meshID]->pMesh) };
			pTempLine->StoreLengthsFromInstances(pLine->GetLength());
			pTempLine->StorePositionsFromInstances(pParentGO->GetComponent<TransformComponent>()->GetWorldPosition());
		}

		DELETE_POINTER(pLine);

		pMesh = m_p3DMeshes[meshID]->pMesh;

		// Check if the instance counter is on 0 if so set to 1 to start
		if (pMesh->GetAmountInstances() <= 0)
			pMesh->IncrementInstanceCount();

		pMesh->IncrementInstanceCount();
		return nullptr;
		//return pMesh;
	}

	// Store the mesh
	MeshData* pMeshData{ new MeshData() };
	pMeshData->ID = GetFreeMeshID();
	pMeshData->name = "";
	pMeshData->pMesh = pMesh;
	m_p3DMeshes.push_back(pMeshData);

	dynamic_cast<Line*>(pMeshData->pMesh)->StorePositionsFromInstances(pParentGO->GetComponent<TransformComponent>()->GetWorldPosition());

	// Return the pointer to the mesh
	return pMesh;
}
Texture* ResourceManager::LoadTexture(const std::string& filePath)
{
	int textureID{};
	if (TextureAlreadyLoaded(filePath, textureID))
		return m_pTextures[textureID]->pTexture;

	// If it does not exist
	Texture* pTexture{ new Texture(filePath) };

	// Store the texture
	TextureData* pTextureData{ new TextureData() };
	pTextureData->ID = GetFreeTextureID();
	pTextureData->filePath = filePath;
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

bool ResourceManager::MeshAlreadyParsed(const std::string& newName, int& meshID)
{
	// Check if the map for 3D meshes is empty
	if (m_p3DMeshes.empty())
		return false;

	// Check if you find the name in the map
	auto result{ std::find_if(m_p3DMeshes.begin(), m_p3DMeshes.end(), [&](MeshData* pMeshData)
	{
		if (pMeshData->name == newName)
		{
			meshID = pMeshData->ID;
			return true;
		}
		return false;
	}) };

	// Did you find the mesh already of not
	if (result != m_p3DMeshes.end())
		return true;

	return false;
}
bool ResourceManager::MeshAlreadyLoaded(Mesh_Base* pMeshBase, int& meshID)
{
	// Check if the map for 3D meshes is empty
	if (m_p3DMeshes.empty())
		return false;

	// Check if you find the name in the map
	auto result{ std::find_if(m_p3DMeshes.begin(), m_p3DMeshes.end(), [&](MeshData* pMeshData)
	{
		Line* pLine{ dynamic_cast<Line*>(pMeshData->pMesh) };
		Line* pLineTest{ dynamic_cast<Line*>(pMeshBase) };
		Mesh* pMesh{ dynamic_cast<Mesh*>(pMeshData->pMesh) };
		Mesh* pMeshTest{ dynamic_cast<Mesh*>(pMeshBase) };

		if (pLine && pLine->GetLineType() == pLineTest->GetLineType())
		{
			meshID = pMeshData->ID;
			return true;
		}
		if (pMesh && pMesh == pMeshTest)
		{
			meshID = pMeshData->ID;
			return true;
		}
		return false;
	}) };

	// Did you find the mesh already of not
	if (result != m_p3DMeshes.end())
		return true;

	return false;
}
bool ResourceManager::MaterialAlreadyLoaded(const Material* material_in, int& materialID)
{
	// Check if the map for materials is empty
	if (m_p3DMaterials.empty())
		return false;

	// Check if you find the name in the map
	auto result{ std::find_if(m_p3DMaterials.begin(), m_p3DMaterials.end(), [&](MaterialData* pMaterialData)
	{
		if (pMaterialData->name == material_in->GetName())
		{
			materialID = pMaterialData->ID;
			return true;
		}
		return false;
	}) };
		
	// Did you find the material already of not
	if (result != m_p3DMaterials.end())
		return true;
	
	return false;
}
bool ResourceManager::TextureAlreadyLoaded(const std::string& filePath, int& textureID)
{
	// Check if the map for 3D meshes is empty
	if (m_pTextures.empty())
		return false;

	// Check if you find the filepath in the map
	auto result{ std::find_if(m_pTextures.begin(), m_pTextures.end(), [&](TextureData* pTextureData)
	{ 
			if (pTextureData->filePath == filePath)
			{
				textureID = pTextureData->ID;
				return true;
			}
			return false;
	}) };

	// Did you find the texture already of not
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
