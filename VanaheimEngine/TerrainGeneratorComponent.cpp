#include "VanaheimPCH.h"
#include "TerrainGeneratorComponent.h"

#include <array>

#include "Material_ProcGen.h"
#include "Mesh.h"
#include "Timer.h"
#include "Octree.h"
#include "KDTree.h"
#include "Voxel.h"
#include "TerrainSettings.h"

#include "../Vanir/InspectorUI.h"

// Timings
#define Total
#define Average

TerrainGeneratorComponent::TerrainGeneratorComponent()
						  : Component()
						  , m_pModelComponent()
						  , m_Seed(1)
						  , m_Octaves(5)
						  , m_Lacunarity(2.3f)
						  , m_Scale(32.24f)
						  , m_Persistence(0.129f)
						  , m_MapSize(1024, 1024)
{
	m_pNoiseGenerator = Locator::GetGeneratorManagerService()->GetGenerator<NoiseGenerator>();

	// Add Observer
	//Locator::GetUIManagerService()->GetUI<InspectorUI>()->AddObserver(this);

	CreateTerrainRegions();
}

void TerrainGeneratorComponent::Initialize(Scene* /*pParentScene*/)
{
	m_pModelComponent = RequiresComponent<ModelComponent>();
}
void TerrainGeneratorComponent::PostInitialize(Scene* /*pParentScene*/)
{}
void TerrainGeneratorComponent::Update(const float /*elapsedSec*/)
{}
void TerrainGeneratorComponent::FixedUpdate(const float /*timeEachUpdate*/)
{}

void TerrainGeneratorComponent::onNotify(ObserverEvent event)
{
	if (event == ObserverEvent::REBUILD_LANDSCAPE)
	{
		ModelComponent* pModelComponent = m_pParentObject->GetComponent<ModelComponent>();
		Mesh* pMesh{ pModelComponent->GetMesh() };
		if (pMesh)
		{
			// Generate up to date noise
			std::vector<float> noiseMap(m_MapSize.x * m_MapSize.y);
			GenerateNoiseMap(noiseMap);
			GenerateColorMap(noiseMap);

			CreateVertices();
			CreateIndices();

			Material* pMaterial{ m_pModelComponent->GetMaterial() };
			pMesh->Initialize(m_Vertices, m_Indices);
			pMesh->PostInitialize(pMaterial);
		}	
	}
}
void TerrainGeneratorComponent::GenerateTerrain(InspectorUI* pInspectorUI)
{
	// For now exit if there already is a mesh
	Mesh* pMesh{ m_pModelComponent->GetMesh() };
	if(pMesh != nullptr)
	{
		Locator::GetResourceManagerService()->RemoveMesh(pMesh);
	}

	std::vector<float> noiseMap(m_MapSize.x * m_MapSize.y);
	GenerateNoiseMap(noiseMap);
	GenerateColorMap(noiseMap);

	CreateVertices();
	CreateIndices();

	pMesh = new Mesh();
	pMesh->Initialize(m_Vertices, m_Indices);
	Locator::GetResourceManagerService()->Store3DMesh(pMesh, "Landscape_1");

	ResourceManager* pResourceManager{ Locator::GetResourceManagerService() };
	Texture* pNormalTexture{ pResourceManager->LoadTexture("../VanaheimEngine/Resources/Textures/Landscape/noiseMap.bmp") };
	Texture* pColorTexture{ pResourceManager->LoadTexture("../VanaheimEngine/Resources/Textures/Landscape/colorMap.bmp") };

	if (pInspectorUI)
	{
		pInspectorUI->AddObserver(this);
		pInspectorUI->AddObserver(pNormalTexture);
		pInspectorUI->AddObserver(pColorTexture);
	}

	Material_ProcGen* pMaterial = new Material_ProcGen();
	pMaterial->AddTexture(pNormalTexture);
	pMaterial->AddTexture(pColorTexture);

	ModelComponent* pModelComponent{ m_pParentObject->GetComponent<ModelComponent>() };
	pModelComponent->SetMesh(pMesh);
	pModelComponent->AddMaterial(pMaterial);

	pMesh->PostInitialize(pModelComponent->GetMaterial());

	m_pParentObject->GetComponent<RenderComponent>()->EnableRenderComponent();
}


//#define personal
#define personalV2
//#define gemini
void TerrainGeneratorComponent::GenerateNoiseMap(std::vector<float>& noiseMap)
{
	#ifdef personal
	const int width = m_MapSize.x;
	const int height = m_MapSize.y;
	const int totalSize = width * height;

	// Create a collection of offsets to offset the samples ;)
	std::vector<DirectX::XMFLOAT3> offsets{};
	GenerateOffsets(offsets, {});

	// Single loop iteration
	for (int i = 0; i < totalSize; ++i)
	{
		// Derive 2D coordinates from 1D index
		const float x = static_cast<float>(i % width);
		const float y = static_cast<float>(i / width);

		const float fractalNoise = m_pNoiseGenerator->FractalNoise({ x, 1.0f, y }, m_Scale, offsets, (float)m_Octaves, m_Lacunarity, m_Persistence);
		const float billowNoise = m_pNoiseGenerator->BillowNoise(fractalNoise);
		const float ridgedNoise = m_pNoiseGenerator->RidgedNoise(billowNoise);

		noiseMap[i] = ridgedNoise;
	}

	// Update your helper to accept the 1D vector
	Normalize1DVector(noiseMap);
	ImageGenerator::GenerateImage(noiseMap, "../VanaheimEngine/Resources/Textures/Landscape/noiseMap.bmp", m_MapSize);
	#endif

	#ifdef personalV2
	const int width = m_MapSize.x;
	const int height = m_MapSize.y;
	const int totalSize = width * height;

	// Create a collection of offsets to offset the samples ;)
	std::vector<DirectX::XMFLOAT3> offsets{};
	GenerateOffsets(offsets, {});

	// Single loop iteration
	for (int i = 0; i < totalSize; ++i)
	{
		// Derive 2D coordinates from 1D index
		const float x = static_cast<float>(i % width);
		const float y = static_cast<float>(i / width);

		//const float fractalNoise = m_pNoiseGenerator->FractalNoise({ x, 1.0f, y }, m_Scale, offsets, (float)m_Octaves, m_Lacunarity, m_Persistence);
		float fractalNoise{};

		float frequency{ 1 };
		float amplitude{ 1 };

		// Calculate the half of the map so that the noiseScale zooms in on the center
		for (size_t j{}; j < m_Octaves; ++j)
		{
			float sampleX{ x / m_Scale * frequency };
			float sampleY{ 1.f / m_Scale * frequency };
			float sampleZ{ y / m_Scale * frequency };

			// Range is [0,1]
			float perlinNoise{ m_pNoiseGenerator->PerlinNoise(sampleX, sampleY, sampleZ) };
			// Range is [-1,1]
			perlinNoise *= 2 - 1;

			fractalNoise += perlinNoise * amplitude;

			frequency *= m_Lacunarity;
			amplitude *= m_Persistence;
		}

		const float billowNoise = abs(fractalNoise);
		const float ridgedNoise = 1 - billowNoise;

		noiseMap[i] = ridgedNoise;
	}

	// Update your helper to accept the 1D vector
	Normalize1DVector(noiseMap);
	ImageGenerator::GenerateImage(noiseMap, "../VanaheimEngine/Resources/Textures/Landscape/noiseMap.bmp", m_MapSize);
	#endif

	#ifdef gemini
	const int width = m_MapSize.x;
	const int height = m_MapSize.y;

	// Create a collection of offsets to offset the samples ;)
	std::vector<DirectX::XMFLOAT3> offsets{};
	GenerateOffsets(offsets, {});

	// 1. Pre-calculate Octave offsets and weights to save math inside the loop
	struct OctaveData { float freq; float amp; DirectX::XMFLOAT3 offset; };
	std::vector<OctaveData> octaveParams(m_Octaves);
	float curFreq = 1.0f;
	float curAmp = 1.0f;
	for (size_t i = 0; i < m_Octaves; ++i) {
		octaveParams[i] = { curFreq, curAmp, offsets[i] };
		curFreq *= m_Lacunarity;
		curAmp *= m_Persistence;
	}

	// 2. Parallel Processing (C++17)
	// If you don't have C++17, use a standard nested for-loop
	#pragma omp parallel for schedule(static) // If using OpenMP
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float noiseValue = 0.0f;

			// Inline fractal logic here or call an optimized version
			for (const auto& opt : octaveParams)
			{
				float sampleX = (x + opt.offset.x) / m_Scale * opt.freq;
				float sampleY = (y + opt.offset.z) / m_Scale * opt.freq;

				// Get raw noise in [-1, 1] range
				float p = m_pNoiseGenerator->PerlinNoise(sampleX, 1.0f, sampleY);
				//float b = m_pNoiseGenerator->BillowNoise(p);

				// For Ridged Noise: apply per octave
				p = 1.0f - std::abs(p);
				p *= p; // Sharpen the ridges

				noiseValue += p * opt.amp;
			}

			noiseMap[y * width + x] = noiseValue;
		}
	}

	Normalize1DVector(noiseMap);
	// Image generation is slow; consider doing this only when needed
	ImageGenerator::GenerateImage(noiseMap, "../VanaheimEngine/Resources/Textures/Landscape/noiseMap.bmp", m_MapSize);
	#endif
}
void TerrainGeneratorComponent::GenerateColorMap(const std::vector<float>& noiseMap)
{
	const int width = m_MapSize.x;
	const int height = m_MapSize.y;
	const int totalSize = width * height;

	// 1. Pre-allocate the flat color map
	std::vector<DirectX::XMFLOAT3> colorMap(totalSize);

	// 2. Single loop iteration
	for (int i = 0; i < totalSize; ++i)
	{
		// In a 1D vector, noiseMap[i] corresponds to colorMap[i]
		const float currentHeight = noiseMap[i];

		// Find the matching terrain region based on height
		for (const auto& region : m_TerrainRegions)
		{
			if (currentHeight <= region.height)
			{
				colorMap[i] = region.color;
				break;
			}
		}
	}

	// 3. Texture Generation
	// Ensure ImageGenerator::GenerateImage is updated to take std::vector<DirectX::XMFLOAT3>
	ImageGenerator::GenerateImage(colorMap, "../VanaheimEngine/Resources/Textures/Landscape/colorMap.bmp", m_MapSize);
}

void TerrainGeneratorComponent::GenerateOffsets(std::vector<DirectX::XMFLOAT3>& offsets, const DirectX::XMFLOAT3& personalOffset)
{
	srand(m_Seed);
	for (size_t i{}; i < m_Octaves; ++i)
	{
		DirectX::XMFLOAT3 offset{};
		offset.x = (float)RandomInt(-100, 100) + personalOffset.x;
		offset.y = (float)RandomInt(-100, 100) + personalOffset.y;
		offset.z = (float)RandomInt(-100, 100) + personalOffset.z;
		offsets.push_back(offset);
	}
}

void TerrainGeneratorComponent::CreateVertices()
{
	m_Vertices.clear();

	const int width = m_MapSize.x;
	const int depth = m_MapSize.y;
	const int totalVertices = width * depth;

	// Guard against empty maps or single-point dimensions
	if (totalVertices <= 0) return;

	const float spacing = 0.1f;

	try
	{
		// Use resize instead of reserve if you want to use direct indexing
		m_Vertices.resize(totalVertices);
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << "Allocation failed: " << e.what() << std::endl;
		return;
	}

	// Pre-calculate divisors to avoid repeated subtractions and divisions
	const float invWidth = (width > 1) ? 1.0f / (width - 1) : 0.0f;
	const float invDepth = (depth > 1) ? 1.0f / (depth - 1) : 0.0f;

	for (int i = 0; i < totalVertices; ++i)
	{
		const int x = i % width;
		const int z = i / width;

		// Use direct indexing for a slight performance boost over push_back
		m_Vertices[i].Position = DirectX::XMFLOAT3(x * spacing, 0.0f, -(z * spacing));

		// UVs: Protected against division by zero
		m_Vertices[i].UV = DirectX::XMFLOAT2(x * invWidth, z * invDepth);
	}
}
void TerrainGeneratorComponent::CreateIndices()
{
	m_Indices.clear();

	const int width{ static_cast<int>(m_MapSize.x) };
	const int height{ static_cast<int>(m_MapSize.y) };

	// Number of quads is (width-1) * (height-1)
	const int numQuads = (width - 1) * (height - 1);

	// Each quad has 2 triangles, each triangle has 3 indices = 6 indices per quad
	m_Indices.reserve(numQuads * 6);

	for (int i = 0; i < numQuads; ++i)
	{
		// Calculate the top-left vertex index of the current quad
		// We use (width - 1) for the modulo to skip the last vertex of each row
		const int x = i % (width - 1);
		const int z = i / (width - 1);

		const uint32_t topLeft = static_cast<uint32_t>(z * width + x);
		const uint32_t topRight = topLeft + 1;
		const uint32_t bottomLeft = topLeft + width;
		const uint32_t bottomRight = bottomLeft + 1;

		// Triangle 1
		m_Indices.push_back(topLeft);
		m_Indices.push_back(bottomRight);
		m_Indices.push_back(bottomLeft);

		// Triangle 2
		m_Indices.push_back(bottomRight);
		m_Indices.push_back(topLeft);
		m_Indices.push_back(topRight);
	}
}
void TerrainGeneratorComponent::CreateTerrainRegions()
{
	m_TerrainRegions.push_back({ TerrainType::WATER_DEEP   , 0.2f, DirectX::Colors::DarkBlue });
	m_TerrainRegions.push_back({ TerrainType::WATER_SHALLOW, 0.4f, DirectX::Colors::Blue });
	m_TerrainRegions.push_back({ TerrainType::SAND		   , 0.45f, DirectX::Colors::LightGoldenrodYellow });
	m_TerrainRegions.push_back({ TerrainType::GRASS		   , 0.50f, DirectX::Colors::LightGreen });
	m_TerrainRegions.push_back({ TerrainType::GRASS_DENSE  , 0.6f, DirectX::Colors::DarkGreen });
	m_TerrainRegions.push_back({ TerrainType::ROCK		   , 0.85f, DirectX::Colors::Brown });
	m_TerrainRegions.push_back({ TerrainType::SNOW		   , 1.f , DirectX::Colors::White });
}