#include "VanaheimPCH.h"
#include "SceneSerializer.h"

#include "yaml-cpp/yaml.h"
#include <fstream>

#include "Scene.h"
#include "Mesh.h"
#include "Graphics.h"

#include "Material_Basic.h"
#include "Material_GPUInstance.h"
#include "Material_ProcGen.h"
#include "Material_ProcGen_GPU.h"

#include "OBJWriter.h"
#include "Texture.h"

namespace YAML
{
	template<>
	struct convert<DirectX::XMFLOAT2>
	{
		static Node encode(const DirectX::XMFLOAT2& rhs)
		{
			Node node{};
			node.push_back(rhs.x);
			node.push_back(rhs.y);
		}
		static bool decode(const Node& pNode, DirectX::XMFLOAT2& rhs)
		{
			if (!pNode.IsSequence() || pNode.size() != 2)
				return false;

			rhs.x = pNode[0].as<float>();
			rhs.y = pNode[1].as<float>();

			return true;
		}
	};
	template<>
	struct convert<DirectX::XMINT2>
	{
		static Node encode(const DirectX::XMINT2& rhs)
		{
			Node node{};
			node.push_back(rhs.x);
			node.push_back(rhs.y);
		}
		static bool decode(const Node& pNode, DirectX::XMINT2& rhs)
		{
			if (!pNode.IsSequence() || pNode.size() != 2)
				return false;

			rhs.x = pNode[0].as<int>();
			rhs.y = pNode[1].as<int>();

			return true;
		}
	};
	template<>
	struct convert<DirectX::XMFLOAT3>
	{
		static Node encode(const DirectX::XMFLOAT3& rhs)
		{
			Node node{};
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
		}
		static bool decode(const Node& pNode, DirectX::XMFLOAT3& rhs)
		{
			if (!pNode.IsSequence() || pNode.size() != 3)
				return false;

			rhs.x = pNode[0].as<float>();
			rhs.y = pNode[1].as<float>();
			rhs.z = pNode[2].as<float>();

			return true;
		}
	};
	template<>
	struct convert<DirectX::XMFLOAT4>
	{
		static Node encode(const DirectX::XMFLOAT4& rhs)
		{
			Node node{};
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
		}
		static bool decode(const Node& pNode, DirectX::XMFLOAT4& rhs)
		{
			if (!pNode.IsSequence() || pNode.size() != 4)
				return false;

			rhs.x = pNode[0].as<float>();
			rhs.y = pNode[1].as<float>();
			rhs.z = pNode[2].as<float>();
			rhs.w = pNode[3].as<float>();

			return true;
		}
	};
	template<>
	struct convert<DirectX::XMFLOAT4X4>
	{
		static Node encode(const DirectX::XMFLOAT4X4& rhs)
		{
			Node node{};
			node.push_back(rhs._11); node.push_back(rhs._21); node.push_back(rhs._31); node.push_back(rhs._41);
			node.push_back(rhs._12); node.push_back(rhs._22); node.push_back(rhs._32); node.push_back(rhs._42);
			node.push_back(rhs._13); node.push_back(rhs._23); node.push_back(rhs._33); node.push_back(rhs._43);
			node.push_back(rhs._14); node.push_back(rhs._24); node.push_back(rhs._34); node.push_back(rhs._44);
		}
		static bool decode(const Node& pNode, DirectX::XMFLOAT4X4& rhs)
		{
			if (!pNode.IsSequence() || pNode.size() != 16)
				return false;

			rhs._11 = pNode[0].as<float>(); 
			rhs._12 = pNode[1].as<float>(); 
			rhs._13 = pNode[2].as<float>();  
			rhs._14 = pNode[3].as<float>();

			rhs._22 = pNode[4].as<float>(); 
			rhs._22 = pNode[5].as<float>();
			rhs._23 = pNode[6].as<float>();  
			rhs._24 = pNode[7].as<float>();

			rhs._33 = pNode[8].as<float>(); 
			rhs._32 = pNode[9].as<float>(); 
			rhs._33 = pNode[10].as<float>(); 
			rhs._34 = pNode[11].as<float>();

			rhs._44 = pNode[12].as<float>(); 
			rhs._42 = pNode[13].as<float>(); 
			rhs._43 = pNode[14].as<float>(); 
			rhs._44 = pNode[15].as<float>();

			return true;
		}
	};
	template<>
	struct convert<std::vector<Texture*>>
	{
		static Node encode(const std::vector<Texture*>& pRhs)
		{
			Node node{};

			const size_t textureCount{ pRhs.size() };
			for (size_t i{}; i < textureCount; ++i)
			{
				node.push_back(pRhs[i]->GetFilePath());
			}
		}
		static bool decode(const Node& pNode, std::vector<Texture*>& pRhs)
		{
			if (!pNode.IsSequence() || pNode.size() != 2)
				return false;
			
			ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };
			const size_t nodeSize{ pNode.size() };
			for (size_t i{}; i < nodeSize; ++i)
			{
				pRhs.push_back(pResourceManager->LoadTexture(pNode[i].as<std::string>()));
			}

			return true;
		}
	};
}

YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT2& f)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << f.x << f.y << YAML::EndSeq;

	return out;
}
YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMINT2& f)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << f.x << f.y << YAML::EndSeq;

	return out;
}
YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT3& f)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << f.x << f.y << f.z << YAML::EndSeq;

	return out;
}
YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4& f)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << f.x << f.y << f.z << f.w << YAML::EndSeq;

	return out;
}
YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4X4& f)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << f._11 
						  << f._12 
						  << f._13 
						  << f._14
						  
						  << f._21 
						  << f._22 
						  << f._23 
						  << f._24
						    
						  << f._31 
						  << f._32 
						  << f._33 
						  << f._34						  
						    
						  << f._41 
						  << f._42 
						  << f._43 
						  << f._44 
						  << YAML::EndSeq;

	return out;
}

void SceneSerializer::Serialize(const std::string& filePath, const Scene* pScene)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << pScene->m_Name;
	out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;

	// Game objects
	const std::vector<GameObject*>& pGameObjects{ pScene->GetObjects() };
	for (GameObject* pGameObject : pGameObjects)
	{
		if (!pGameObject)
			return;

		SerializeGameObject(out, pGameObject);
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filePath);
	if (fout.is_open())
		fout << out.c_str();
	else
		LOG_ERROR("Filepath " + filePath + " does not exist.");
}
bool SceneSerializer::Deserialize(const std::string& filePath, Scene* pScene)
{	
	// Open files stream
	std::ifstream stream(filePath);
	std::stringstream sstream;
	sstream << stream.rdbuf();

	// Load data
	YAML::Node data = YAML::Load(sstream.str());
	if (!data["Scene"])
		return false;

	// Load sceneName
	std::string sceneName{ data["Scene"].as<std::string>() };
	LOG(ErrorLevel::LOG_INFO, "Deserializing scene '{0}'" + sceneName);
	pScene->m_Name = sceneName;

	// Load gameobjects
	const YAML::Node gameObjectData = data["GameObjects"];
	if (gameObjectData)
	{
		// Loop over all gameobjects
		for (YAML::detail::iterator_value yamlGO : gameObjectData)
		{
			//uint64_t uuid = yamlGO["GameObject"].as<uint64_t>();
			
			GameObject* pGO = new GameObject("");

			DeserializeNameComponent(yamlGO, pGO);
			DeserializeTransformComponent(yamlGO, pGO);
			DeserializeCameraComponent(yamlGO, pGO);
			DeserializeRenderComponent(yamlGO, pGO);
			DeserializeModelComponent(yamlGO, pGO);
			DeserializeTerrainGeneratorComponent(yamlGO, pGO);
			
			pGO->Initialize();
			pGO->PostInitialize();

			pScene->AddGameObject(pGO);
		}
	}

	Notify(ObserverEvent::REBUILD_LANDSCAPE);

	return true;
}

// --------------------
// Serializing
// --------------------
void SceneSerializer::SerializeGameObject(YAML::Emitter& out, GameObject* pGameObject)
{
	out << YAML::BeginMap;
	out << YAML::Key << "GameObject" << YAML::Value << "1235456871354987412";

	if (pGameObject->HasComponent<NameComponent>())
	{
		NameComponent* pNameComponent{ pGameObject->GetComponent<NameComponent>() };
		SerializeNameComponent(out, pNameComponent);		
	}
	if (pGameObject->HasComponent<TransformComponent>())
	{
		TransformComponent* pTransformComponent{ pGameObject->GetComponent<TransformComponent>() };
		SerializeTransformComponent(out, pTransformComponent);		
	}
	if (pGameObject->HasComponent<CameraComponent>())
	{
		CameraComponent* pCameraComponent{ pGameObject->GetComponent<CameraComponent>() };
		SerializeCameraComponent(out, pCameraComponent);
	}
	if (pGameObject->HasComponent<RenderComponent>())
	{
		RenderComponent* pRenderComponent{ pGameObject->GetComponent<RenderComponent>() };
		SerializeRenderComponent(out, pRenderComponent);
	}
	if (pGameObject->HasComponent<ModelComponent>())
	{
		ModelComponent* pModelComponent{ pGameObject->GetComponent<ModelComponent>() };
		SerializeModelComponent(out, pModelComponent);
	}
	if (pGameObject->HasComponent<TerrainGeneratorComponent>())
	{
		TerrainGeneratorComponent* pTerrainGeneratorComponent{ pGameObject->GetComponent<TerrainGeneratorComponent>() };
		SerializeTerrainGeneratorComponent(out, pTerrainGeneratorComponent);
	}

	out << YAML::EndMap;
}
void SceneSerializer::SerializeNameComponent(YAML::Emitter& out, NameComponent* pNameComponent)
{
	out << YAML::Key << "NameComponent";
	out << YAML::BeginMap;
	out << YAML::Key << "Name" << YAML::Value << pNameComponent->m_Name;
	out << YAML::EndMap;
}
void SceneSerializer::SerializeTransformComponent(YAML::Emitter& out, TransformComponent* pTransformComponent)
{
	out << YAML::Key << "TransformComponent";
	out << YAML::BeginMap;

	out << YAML::Key << "LocalPosition" << YAML::Value << pTransformComponent->m_Position;
	out << YAML::Key << "LocalRotation" << YAML::Value << pTransformComponent->GetRotation(false);
	out << YAML::Key << "LocalScale" << YAML::Value << pTransformComponent->m_Scale;

	out << YAML::Key << "WorldPosition" << YAML::Value << pTransformComponent->m_WorldPosition;
	out << YAML::Key << "WorldRotation" << YAML::Value << pTransformComponent->m_WorldRotation;
	out << YAML::Key << "WorldScale" << YAML::Value << pTransformComponent->m_WorldScale;

	out << YAML::Key << "Forward" << YAML::Value << pTransformComponent->m_Forward;
	out << YAML::Key << "Right" << YAML::Value <<	pTransformComponent->m_Right;
	out << YAML::Key << "Up" << YAML::Value <<		pTransformComponent->m_Up;
	out << YAML::Key << "WorldUp" << YAML::Value << pTransformComponent->m_WorldUp;

	out << YAML::Key << "World" << YAML::Value << pTransformComponent->m_World;

	out << YAML::EndMap;
}
void SceneSerializer::SerializeCameraComponent(YAML::Emitter& out, CameraComponent* pCameraComponent)
{
	out << YAML::Key << "CameraComponent";
	out << YAML::BeginMap;
	out << YAML::Key << "IsMainCamera" << YAML::Value << pCameraComponent->m_IsSceneCamera;

	out << YAML::Key << "Near" << YAML::Value << pCameraComponent->m_Near;
	out << YAML::Key << "Far" << YAML::Value << pCameraComponent->m_Far;
	out << YAML::Key << "FOV" << YAML::Value << pCameraComponent->m_FOV;

	out << YAML::Key << "View" << YAML::Value << pCameraComponent->m_View;
	out << YAML::Key << "Projection" << YAML::Value << pCameraComponent->m_Projection;
	out << YAML::Key << "ViewProjection" << YAML::Value << pCameraComponent->m_ViewProjection;
	out << YAML::Key << "ViewInverse" << YAML::Value << pCameraComponent->m_ViewInverse;
	out << YAML::Key << "ViewProjectionInverse" << YAML::Value << pCameraComponent->m_ViewProjectionInverse;
	out << YAML::EndMap;
}
void SceneSerializer::SerializeRenderComponent(YAML::Emitter& out, RenderComponent* pRenderComponent)
{
	out << YAML::Key << "RenderComponent";
	out << YAML::BeginMap;
	out << YAML::Key << "RenderComponent" << YAML::Value << int(pRenderComponent->m_CanRenderComponent);
	out << YAML::EndMap;
}
void SceneSerializer::SerializeModelComponent(YAML::Emitter& out, ModelComponent* pModelComponent)
{
	out << YAML::Key << "ModelComponent";
	out << YAML::BeginMap;

	SerializeMesh(out, pModelComponent);
	SerializeMaterial(out, pModelComponent);
	SerializeTexture(out, pModelComponent);
	out << YAML::Key << "FilePath" << YAML::Value << pModelComponent->GetFilePath();

	out << YAML::EndMap;
}
void SceneSerializer::SerializeTerrainGeneratorComponent(YAML::Emitter& out, TerrainGeneratorComponent* pTerrainGeneratorComponent)
{
	out << YAML::Key << "TerrainGeneratorComponent";
	out << YAML::BeginMap;

	out << YAML::Key << "Seed" << YAML::Value << pTerrainGeneratorComponent->m_Seed;
	out << YAML::Key << "Octaves" << YAML::Value << pTerrainGeneratorComponent->m_Octaves;
	out << YAML::Key << "Lacunarity" << YAML::Value << pTerrainGeneratorComponent->m_Lacunarity;
	out << YAML::Key << "Scale" << YAML::Value << pTerrainGeneratorComponent->m_Scale;
	out << YAML::Key << "Persistence" << YAML::Value << pTerrainGeneratorComponent->m_Persistence;
	out << YAML::Key << "Mapsize" << YAML::Value << pTerrainGeneratorComponent->m_MapSize;

	out << YAML::EndMap;
}

void SceneSerializer::SerializeMesh(YAML::Emitter& out, ModelComponent* pModelComponent)
{
	Mesh* pMesh{ pModelComponent->m_pMesh };
	
	out << YAML::Key << "Mesh" << YAML::Value;
	out << YAML::BeginMap;
	const std::string& filePath{ pMesh->m_FilePath };
	if (filePath == "")
	{
		ID3D11Buffer* pVBuffer{ pMesh->m_pVBuffer };
		size_t sizeVBuffer{ pMesh->m_AmountVertices * sizeof(Vertex_Input) };
		const std::vector<Vertex_Input>& vertices{ ReadBufferData<Vertex_Input>(pVBuffer, sizeVBuffer, pMesh->m_AmountVertices) };

		ID3D11Buffer* pIBuffer{ pMesh->m_pIBuffer };
		size_t sizeIBuffer{ pMesh->m_AmountIndices * sizeof(uint32_t) };
		const std::vector<uint32_t>& indices{ ReadBufferData<uint32_t>(pIBuffer, sizeIBuffer, pMesh->m_AmountIndices) };

		OBJWriter objWriter{};
		const std::string newFilePath{ objWriter.WriteOBJ("Crap", "./Resources/Meshes/", vertices, indices) };

		out << YAML::Key << "FilePath" << YAML::Value << newFilePath;
	}
	else
	{
		out << YAML::Key << "FilePath" << YAML::Value << pMesh->m_FilePath;
	}

	out << YAML::EndMap;
}
void SceneSerializer::SerializeMaterial(YAML::Emitter& out, ModelComponent* pModelComponent)
{
	Material* pMaterial{ pModelComponent->m_pMaterial };	
	Material_ProcGen* pMaterialPCG{ CastMaterial<Material_ProcGen>(pMaterial) };

	out << YAML::Key << "Material" << YAML::Value;
	out << YAML::BeginMap;
	out << YAML::Key << "MaterialType" << YAML::Value << typeid(Material_ProcGen).name();
	out << YAML::Key << "MaterialName" << YAML::Value << pMaterialPCG->m_Name;
	out << YAML::Key << "MaterialPath" << YAML::Value << pMaterialPCG->m_AssetPath;
	out << YAML::EndMap;
}
void SceneSerializer::SerializeTexture(YAML::Emitter& out, ModelComponent* pModelComponent)
{
	Material* pMaterial{ pModelComponent->m_pMaterial };
	Material_ProcGen* pMaterialPCG{ CastMaterial<Material_ProcGen>(pMaterial) };
	const std::vector<Texture*> pTextures{ pMaterialPCG->m_pTextures };

	out << YAML::Key << "Textures" << YAML::Value;
	out << YAML::Flow;
	out << YAML::BeginSeq; 
	const size_t textureCount{ pTextures.size() };
	for (size_t i = 0; i < textureCount; ++i)
	{
		out << pTextures[i]->m_Path;
	}
	out << YAML::EndSeq;
}

void SceneSerializer::SerializeID3D11Buffer_Vertices(YAML::Emitter& out, const std::string& bufferName, uint32_t typeAmount, Mesh* pMesh)
{
	// Reference: https://www.gamedev.net/forums/topic/608576-writing-a-d3dx11buffer-to-a-file/
	// Reference: https://stackoverflow.com/questions/62563764/read-back-vertex-buffers-from-gpu-in-directx-11-and-get-vertices
	out << YAML::Key << bufferName << YAML::Value;
	out << YAML::BeginMap;

	ID3D11Buffer* pBuffer{ pMesh->GetVBuffer() };
	size_t size{ typeAmount * sizeof(Vertex_Input) };
	const std::vector<Vertex_Input>& vertices{ ReadBufferData<Vertex_Input>(pBuffer, size, typeAmount) };

	for (size_t i = 0; i < typeAmount; ++i)
	{
		const Vertex_Input& localVertex{ vertices[i] };

		out << YAML::Key << "Position" << YAML::Value << localVertex.Position;
		out << YAML::Key << "Color" << YAML::Value << localVertex.Color;
		out << YAML::Key << "UV" << YAML::Value << localVertex.UV;
		out << YAML::Key << "InstancePos" << YAML::Value << localVertex.InstancePos;
		out << YAML::Key << "InstanceLengthV1" << YAML::Value << localVertex.InstanceLengthV1;
		out << YAML::Key << "InstanceLengthV2" << YAML::Value << localVertex.InstanceLengthV2;
	}
	
	out << YAML::EndMap;
}
void SceneSerializer::SerializeID3D11Buffer_Indicies(YAML::Emitter& out, const std::string& bufferName, uint32_t typeAmount, Mesh* pMesh)
{
	// Reference: https://www.gamedev.net/forums/topic/608576-writing-a-d3dx11buffer-to-a-file/
	// Reference: https://stackoverflow.com/questions/62563764/read-back-vertex-buffers-from-gpu-in-directx-11-and-get-vertices
	out << YAML::Key << bufferName << YAML::Value;
	out << YAML::BeginMap;

	ID3D11Buffer* pBuffer{ pMesh->GetIBuffer() };
	size_t size{ typeAmount * sizeof(uint32_t) };
	std::vector<uint32_t> indices{ ReadBufferData<uint32_t>(pBuffer, size, typeAmount) };

	for (size_t i = 0; i < typeAmount; ++i)
	{
		out << YAML::Key << "Index" << YAML::Value << indices[i];
	}

	out << YAML::EndMap;
}

// --------------------
// Deserializing
// --------------------
void SceneSerializer::DeserializeNameComponent(const YAML::detail::iterator_value& yamlGO, GameObject* pGO)
{
	// ------------------------------------------------------------------------------------------------------------
	// Name Component
	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------
	/** Get */	
	const YAML::Node nameComponent = yamlGO["NameComponent"];
	if (nameComponent)
	{
		/** Gather */
		const std::string name{ nameComponent["Name"].as<std::string>() };

		/** Create */
		NameComponent* pNameComponent = pGO->GetComponent<NameComponent>();

		/** Add */
		pNameComponent->m_Name = name;
	}
	
}
void SceneSerializer::DeserializeTransformComponent(const YAML::detail::iterator_value& yamlGO, GameObject* pGO)
{
	// ------------------------------------------------------------------------------------------------------------
	// Transform Component
	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------
	/** Get */
	YAML::Node transformComponent = yamlGO["TransformComponent"];		
	if (transformComponent)
	{
		/** Gather */
		DirectX::XMFLOAT3 position{ transformComponent["LocalPosition"].as<DirectX::XMFLOAT3>() };
		DirectX::XMFLOAT3 worldPosition{ transformComponent["WorldPosition"].as<DirectX::XMFLOAT3>() };
		DirectX::XMFLOAT3 scale{ transformComponent["LocalScale"].as<DirectX::XMFLOAT3>() };
		DirectX::XMFLOAT3 worldScale{ transformComponent["WorldScale"].as<DirectX::XMFLOAT3>() };
		DirectX::XMFLOAT4 rotation{ transformComponent["LocalRotation"].as<DirectX::XMFLOAT4>() };
		DirectX::XMFLOAT4 worldRotation{ transformComponent["WorldRotation"].as<DirectX::XMFLOAT4>() };
		DirectX::XMFLOAT3 forward{ transformComponent["Forward"].as<DirectX::XMFLOAT3>() };
		DirectX::XMFLOAT3 right{ transformComponent["Right"].as<DirectX::XMFLOAT3>() };
		DirectX::XMFLOAT3 up{ transformComponent["Up"].as<DirectX::XMFLOAT3>() };
		DirectX::XMFLOAT3 worldUp{ transformComponent["WorldUp"].as<DirectX::XMFLOAT3>() };
		DirectX::XMFLOAT4X4 world{ transformComponent["World"].as<DirectX::XMFLOAT4X4>() };

		/** Create */
		TransformComponent* pTransformComponent{ pGO->GetComponent<TransformComponent>() };

		/** Add */
		pTransformComponent->m_Position = position;
		pTransformComponent->m_WorldPosition = worldPosition;
		pTransformComponent->m_Scale = scale;
		pTransformComponent->m_WorldScale = worldScale;
		pTransformComponent->m_Rotation = rotation;
		pTransformComponent->m_WorldRotation = worldRotation;

		pTransformComponent->m_Forward = forward;
		pTransformComponent->m_Right = right;
		pTransformComponent->m_Up = up;
		pTransformComponent->m_WorldUp = worldUp;

		pTransformComponent->m_World = world;
	}
}
void SceneSerializer::DeserializeCameraComponent(const YAML::detail::iterator_value& yamlGO, GameObject* pGO)
{
	// ------------------------------------------------------------------------------------------------------------
	// Camera Component
	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------
	/** Get */
	YAML::Node cameraComponent = yamlGO["CameraComponent"];	
	if (cameraComponent)
	{
		/** Gather */
		float nearPlane{ cameraComponent["Near"].as<float>() };
		float farPlane{ cameraComponent["Far"].as<float>() };
		float FOV{ cameraComponent["FOV"].as<float>() };
		DirectX::XMFLOAT4X4 view{ cameraComponent["View"].as<DirectX::XMFLOAT4X4>() };
		DirectX::XMFLOAT4X4 projection{ cameraComponent["Projection"].as<DirectX::XMFLOAT4X4>() };
		DirectX::XMFLOAT4X4 viewProjection{ cameraComponent["ViewProjection"].as<DirectX::XMFLOAT4X4>() };
		DirectX::XMFLOAT4X4 viewInverse{ cameraComponent["ViewInverse"].as<DirectX::XMFLOAT4X4>() };
		DirectX::XMFLOAT4X4 viewProjectionInverse{ cameraComponent["ViewProjectionInverse"].as<DirectX::XMFLOAT4X4>() };

		/** Create */
		CameraComponent* pCameraComponent{ new CameraComponent() };

		/** Add */
		pCameraComponent->m_Far = farPlane;
		pCameraComponent->m_Near = nearPlane;
		pCameraComponent->m_FOV = FOV;
		pCameraComponent->m_View = view;
		pCameraComponent->m_Projection = projection;
		pCameraComponent->m_ViewProjection = viewProjection;
		pCameraComponent->m_ViewInverse = viewInverse;
		pCameraComponent->m_ViewProjectionInverse = viewProjectionInverse;

		pGO->AddComponent(pCameraComponent);
	}
}
void SceneSerializer::DeserializeRenderComponent(const YAML::detail::iterator_value& yamlGO, GameObject* pGO)
{
	// ------------------------------------------------------------------------------------------------------------
	// Render Component
	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------
	/** Get */
	YAML::Node renderComponent = yamlGO["RenderComponent"];
	if (renderComponent)
	{
		/** Gather */
		const int canRenderComponent = renderComponent["RenderComponent"].as<int>();

		/** Create */
		RenderComponent* pRenderComponent{ pGO->GetComponent<RenderComponent>() };

		/** Add */
		pRenderComponent->m_CanRenderComponent = bool(canRenderComponent);
	}
}
void SceneSerializer::DeserializeModelComponent(const YAML::detail::iterator_value& yamlGO, GameObject* pGO)
{
	// ------------------------------------------------------------------------------------------------------------
	// Model Component
	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------
	/** Get */
	YAML::Node modelComponent = yamlGO["ModelComponent"];
	if (modelComponent)
	{
		/** Gather */
		// Mesh
		std::string filePath{};
		YAML::Node modelComponent_Mesh = modelComponent["Mesh"];
		if (modelComponent_Mesh)
		{
			filePath = modelComponent_Mesh["FilePath"].as<std::string>();
		}
		// Material
		std::string classType{};
		std::string materialName{};
		std::string materialPath{};
		YAML::Node modelComponent_Material = modelComponent["Material"];
		if (modelComponent_Material)
		{
			classType = modelComponent_Material["MaterialType"].as<std::string>();
			materialName = modelComponent_Material["MaterialName"].as<std::string>();
			materialPath = modelComponent_Material["MaterialPath"].as<std::string>();
		}
		// Textures
		std::vector<Texture*> pTextures{};
		YAML::Node modelComponent_Textures = modelComponent["Textures"];
		if (modelComponent_Textures)
		{
			pTextures = modelComponent["Textures"].as<std::vector<Texture*>>();
		}
		
		/** Create */
		ModelComponent* pModelComponent{ new ModelComponent(filePath) };
		pModelComponent->AddMaterial(CreateMaterial(classType, pTextures));

		/** Add */
		pGO->AddComponent(pModelComponent);
	}
}
void SceneSerializer::DeserializeTerrainGeneratorComponent(const YAML::detail::iterator_value& yamlGO, GameObject* pGO)
{
	// ------------------------------------------------------------------------------------------------------------
	// Terrain Generator Component
	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------
	/** Get */
	YAML::Node terrainGenComponent = yamlGO["TerrainGeneratorComponent"];
	if (terrainGenComponent)
	{
		/** Gather */
		const size_t seed = terrainGenComponent["Seed"].as<size_t>();
		const size_t octaves = terrainGenComponent["Octaves"].as<size_t>();
		const float lacunarity = terrainGenComponent["Lacunarity"].as<float>();
		const float scale = terrainGenComponent["Scale"].as<float>();
		const float persistence = terrainGenComponent["Persistence"].as<float>();
		const DirectX::XMINT2 mapSIze = terrainGenComponent["Mapsize"].as<DirectX::XMINT2>();

		/** Create */
		TerrainGeneratorComponent* pTerrainGeneratorComponent{ new TerrainGeneratorComponent() };
		pTerrainGeneratorComponent->m_Seed = seed;
		pTerrainGeneratorComponent->m_Octaves = octaves;
		pTerrainGeneratorComponent->m_Lacunarity = lacunarity;
		pTerrainGeneratorComponent->m_Scale = scale;
		pTerrainGeneratorComponent->m_Persistence = persistence;
		pTerrainGeneratorComponent->m_MapSize = mapSIze;

		/** Add */
		pGO->AddComponent(pTerrainGeneratorComponent);

		AddObserver(pTerrainGeneratorComponent);

		Locator::GetUIManagerService()->GetUI<InspectorUI>()->AddObserver(pTerrainGeneratorComponent);
	}

}

Material* SceneSerializer::CreateMaterial(const std::string& name, const std::vector<Texture*> pTextures)
{
	if (name == "class Material_ProcGen")
	{
		Material_ProcGen* pMatPCG{ new Material_ProcGen() };

		const size_t textureCount{ pTextures.size() };
		for (size_t i{}; i < textureCount; ++i)
		{
			pMatPCG->AddTexture(pTextures[i]);
		}

		return pMatPCG;
	}

	return nullptr;
}
