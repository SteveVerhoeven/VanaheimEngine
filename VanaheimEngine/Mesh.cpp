// ********************************
// GPU Instancing used
// Reference: https://www.braynzarsoft.net/viewtutorial/q16390-33-instancing-with-indexed-primitives
// ********************************
#include "pch.h"
#include "Mesh.h"

#include "Graphics.h"
#include "OBJParser.h"

#include "Scene.h"
#include "Material.h"
#include "GameObject.h"
#include "SceneManager.h"

#include "DebugLogger.h"

Mesh::Mesh()
	 : Mesh("")
{}
Mesh::Mesh(const std::string& path)
	 : m_IsInitialized(false)
	 , m_IsRendered(false)
	 , m_FilePath(path)
	 , m_AmountIndices()
	 , m_AmountVertices()
	 , m_AmountInstances()
	 , m_pIBuffer(nullptr)
	 , m_pVBuffer(nullptr)
	 , m_pInstanceBuffer(nullptr)
	 , m_pConstantBuffer(nullptr)
	 , m_pInputLayout(nullptr)
	 , m_InstancePositionData(std::vector<DirectX::XMFLOAT3>())
{}

Mesh::~Mesh()
{
	DELETE_RESOURCE(m_pIBuffer);
	DELETE_RESOURCE(m_pVBuffer);
	DELETE_RESOURCE(m_pInstanceBuffer);
	DELETE_RESOURCE(m_pConstantBuffer);
	DELETE_RESOURCE(m_pInputLayout);
}

void Mesh::Initialize()
{
	if (m_IsInitialized)
		return;

	std::vector<Vertex_Input> vertices{};
	std::vector<uint32_t> indices{};

	OBJParser parser{};
	parser.LoadModel("Triangle", m_FilePath, vertices, indices, nullptr);

	DebugLogger* pLogger = Locator::GetDebugLoggerService();
	ID3D11Device* pDevice{ Locator::GetGraphicsService()->GetDevice() };
	pLogger->LogHRESULT(CreateVertexBuffer(pDevice, vertices), "Mesh::CreateVertexBuffer", __FILE__, std::to_string(__LINE__));
	pLogger->LogHRESULT(CreateIndexBuffer(pDevice, indices), "Mesh::CreateIndexBuffer", __FILE__, std::to_string(__LINE__));
}
void Mesh::Initialize(const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices)
{
	DebugLogger* pLogger = Locator::GetDebugLoggerService();
	ID3D11Device* pDevice{ Locator::GetGraphicsService()->GetDevice() };
	pLogger->LogHRESULT(CreateVertexBuffer(pDevice, vertices), "Mesh::CreateVertexBuffer", __FILE__, std::to_string(__LINE__));
	pLogger->LogHRESULT(CreateIndexBuffer(pDevice, indices), "Mesh::CreateIndexBuffer", __FILE__, std::to_string(__LINE__));
}
void Mesh::ReInitialize(const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices)
{
	DELETE_RESOURCE(m_pIBuffer);
	DELETE_RESOURCE(m_pVBuffer);

	DebugLogger* pLogger = Locator::GetDebugLoggerService();
	ID3D11Device* pDevice{ Locator::GetGraphicsService()->GetDevice() };
	pLogger->LogHRESULT(CreateVertexBuffer(pDevice, vertices), "Mesh::CreateVertexBuffer", __FILE__, std::to_string(__LINE__));
	pLogger->LogHRESULT(CreateIndexBuffer(pDevice, indices), "Mesh::CreateIndexBuffer", __FILE__, std::to_string(__LINE__));
}
void Mesh::PostInitialize(Material* pMaterial)
{	
	if (m_IsInitialized)
		return;

	DebugLogger* pLogger = Locator::GetDebugLoggerService();
	ID3D11Device* pDevice{ Locator::GetGraphicsService()->GetDevice() };

	HRESULT hr{};
	hr = CreateVertex_InputLayout(pDevice, pMaterial);
	pLogger->LogHRESULT(hr, "Mesh::CreateVertex_InputLayout", __FILE__, std::to_string(__LINE__));

	hr = CreateInstanceBuffer(pDevice);
	pLogger->LogHRESULT(hr, "Mesh::CreateInstanceBuffer", __FILE__, std::to_string(__LINE__));
	
	hr = CreateConstantBuffer(pDevice);
	pLogger->LogHRESULT(hr, "Mesh::CreateConstantBuffer", __FILE__, std::to_string(__LINE__));
	
	if (m_AmountInstances != 0)
		UpdateBuffer();

	// ********************************
	// Set that the mesh is initialized
	// ********************************
	m_IsInitialized = true;
}

HRESULT Mesh::CreateVertex_InputLayout(ID3D11Device* pDevice, Material* pMaterial)
{
	// ********************************
	// Create vertex input layout
	// ********************************
	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_input_element_desc
	static const uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};
	{
		/* D3D11_INPUT_ELEMENT_DESC - Parameters */
		LPCSTR                     SemanticName{ "POSITION" };
		UINT                       SemanticIndex{ 0 };
		DXGI_FORMAT                Format{ DXGI_FORMAT_R32G32B32_FLOAT };
		UINT                       InputSlot{ 0 };
		UINT                       AlignedByteOffset{ 0 };
		D3D11_INPUT_CLASSIFICATION InputSlotClass{ D3D11_INPUT_PER_VERTEX_DATA };
		UINT                       InstanceDataStepRate{ 0 };
		vertexDesc[0] = { SemanticName, SemanticIndex, Format, InputSlot, AlignedByteOffset, InputSlotClass, InstanceDataStepRate};
	}
	{
		/* D3D11_INPUT_ELEMENT_DESC - Parameters */
		LPCSTR                     SemanticName{ "COLOR" };
		UINT                       SemanticIndex{ 0 };
		DXGI_FORMAT                Format{ DXGI_FORMAT_R32G32B32_FLOAT };
		UINT                       InputSlot{ 0 };
		UINT                       AlignedByteOffset{ 12 };
		D3D11_INPUT_CLASSIFICATION InputSlotClass{ D3D11_INPUT_PER_VERTEX_DATA, };
		UINT                       InstanceDataStepRate{ 0 };
		vertexDesc[1] = { SemanticName, SemanticIndex, Format, InputSlot, AlignedByteOffset, InputSlotClass, InstanceDataStepRate };
	}
	{
		/* D3D11_INPUT_ELEMENT_DESC - Parameters */
		LPCSTR                     SemanticName{ "TEXCOORD" };
		UINT                       SemanticIndex{ 0 };
		DXGI_FORMAT                Format{ DXGI_FORMAT_R32G32_FLOAT };
		UINT                       InputSlot{ 0 };
		UINT                       AlignedByteOffset{ 24 };
		D3D11_INPUT_CLASSIFICATION InputSlotClass{ D3D11_INPUT_PER_VERTEX_DATA, };
		UINT                       InstanceDataStepRate{ 0 };
		vertexDesc[2] = { SemanticName, SemanticIndex, Format, InputSlot, AlignedByteOffset, InputSlotClass, InstanceDataStepRate };
	}
	{	// instance STUFF
		/* D3D11_INPUT_ELEMENT_DESC - Parameters */
		LPCSTR                     SemanticName{ "INSTANCEPOS" };
		UINT                       SemanticIndex{ 0 };
		DXGI_FORMAT                Format{ DXGI_FORMAT_R32G32B32_FLOAT };
		UINT                       InputSlot{ 1 };
		UINT                       AlignedByteOffset{ 0 };
		D3D11_INPUT_CLASSIFICATION InputSlotClass{ D3D11_INPUT_PER_INSTANCE_DATA, };
		UINT                       InstanceDataStepRate{ 1 };
		vertexDesc[3] = { SemanticName, SemanticIndex, Format, InputSlot, AlignedByteOffset, InputSlotClass, InstanceDataStepRate };
	}

	// ********************************
	// Create Input Layout
	// ********************************
	D3DX11_PASS_DESC passDesc{};
	HRESULT hr{ pMaterial->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc) };
	if (FAILED(hr))
		Locator::GetDebugLoggerService()->LogHRESULT(hr, "Mesh::CreateVertex_InputLayout", __FILE__, std::to_string(__LINE__));

	/* CreateInputLayout - Parameters */
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs{ vertexDesc };
	UINT nbrElements{ numElements };
	const void* pShaderBytecodeWithInputSignature{ passDesc.pIAInputSignature };
	size_t bytecodeLength{ passDesc.IAInputSignatureSize };
	ID3D11InputLayout** ppInputLayout{ &m_pInputLayout };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createinputlayout
	return pDevice->CreateInputLayout(pInputElementDescs, nbrElements, pShaderBytecodeWithInputSignature, bytecodeLength, ppInputLayout);
}
HRESULT Mesh::CreateIndexBuffer(ID3D11Device* pDevice, const std::vector<uint32_t>& iBuffer)
{
	// ********************************
	// Create Index Buffer
	// ********************************
	m_AmountIndices = (uint32_t)iBuffer.size();
	D3D11_BUFFER_DESC ibd{};
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData_IBuffer{};
	ZeroMemory(&initData_IBuffer, sizeof(initData_IBuffer));
	initData_IBuffer.pSysMem = iBuffer.data();

	/* CreateBuffer - Parameters */
	const D3D11_BUFFER_DESC* pIBufferDesc{ &ibd };
	const D3D11_SUBRESOURCE_DATA* pInitialData_IBuffer{ &initData_IBuffer };
	ID3D11Buffer** ppIBuffer{ &m_pIBuffer };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer
	return pDevice->CreateBuffer(pIBufferDesc, pInitialData_IBuffer, ppIBuffer);
}
HRESULT Mesh::CreateVertexBuffer(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vBuffer)
{
	// ********************************
	// Create vertex buffer
	// ********************************
	m_AmountVertices = (uint32_t)vBuffer.size();
	D3D11_BUFFER_DESC vbd{};
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex_Input) * (uint32_t)vBuffer.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData_VBuffer{};
	ZeroMemory(&initData_VBuffer, sizeof(initData_VBuffer));
	initData_VBuffer.pSysMem = vBuffer.data();

	/* CreateBuffer - Parameters */
	const D3D11_BUFFER_DESC* pVBufferDesc{ &vbd };
	const D3D11_SUBRESOURCE_DATA* pInitialData_VBuffer{ &initData_VBuffer };
	ID3D11Buffer** ppVBuffer{ &m_pVBuffer };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer
	return pDevice->CreateBuffer(pVBufferDesc, pInitialData_VBuffer, ppVBuffer);
}

HRESULT Mesh::CreateInstanceBuffer(ID3D11Device* pDevice)
{
	// ********************************
	// Create instance buffer: Data
	// ********************************
	Scene* pScene{ Locator::GetSceneManagerService()->GetActiveGameScene() };
	std::vector<GameObject*> pGameObjects{ pScene->GetObjects() };
	for (GameObject* pGameObject : pGameObjects)
	{
		ModelComponent* pModelComponent{ pGameObject->GetComponent<ModelComponent>() };
		if (!pModelComponent)
			continue;

		Mesh* pMesh{ pModelComponent->GetMesh() };
		if (pMesh != this)
			continue;

		TransformComponent* pTransformComponent{ pGameObject->GetComponent<TransformComponent>() };
		m_InstancePositionData.push_back(pTransformComponent->GetWorldPosition());
	}

	// If there is only 1 instance of that mesh than clear out the data vector so it won't render as an instance
	if (m_InstancePositionData.size() <= 1)
	{
		m_InstancePositionData.clear();		
		return S_OK;
	}

	// ********************************
	// Create instance buffer: DirectX
	// ********************************
	D3D11_BUFFER_DESC instBuffDesc;
	ZeroMemory(&instBuffDesc, sizeof(instBuffDesc));
	instBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	instBuffDesc.ByteWidth = sizeof(DirectX::XMFLOAT3) * m_AmountInstances;
	instBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instBuffDesc.CPUAccessFlags = 0;
	instBuffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA instData;
	ZeroMemory(&instData, sizeof(instData));
	instData.pSysMem = m_InstancePositionData.data();

	/* CreateBuffer - Parameters */
	const D3D11_BUFFER_DESC* pInstanceBufferDesc{ &instBuffDesc };
	const D3D11_SUBRESOURCE_DATA* pInitialData_InstanceBuffer{ &instData };
	ID3D11Buffer** ppInstanceBuffer{ &m_pInstanceBuffer };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer
	return pDevice->CreateBuffer(pInstanceBufferDesc, pInitialData_InstanceBuffer, ppInstanceBuffer);
}
HRESULT Mesh::CreateConstantBuffer(ID3D11Device* pDevice)
{
	// ********************************
	// Create instance buffer: DirectX
	// ********************************
	D3D11_BUFFER_DESC constantBuffDesc;
	ZeroMemory(&constantBuffDesc, sizeof(constantBuffDesc));
	constantBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBuffDesc.ByteWidth = sizeof(DirectX::XMFLOAT4);
	constantBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBuffDesc.CPUAccessFlags = 0;
	constantBuffDesc.MiscFlags = 0;

	/* CreateBuffer - Parameters */
	const D3D11_BUFFER_DESC* pCBufferDesc{ &constantBuffDesc };
	const D3D11_SUBRESOURCE_DATA* pInitialData_CBuffer{ nullptr };
	ID3D11Buffer** ppCBuffer{ &m_pConstantBuffer };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer
	return pDevice->CreateBuffer(pCBufferDesc, pInitialData_CBuffer, ppCBuffer);
}

void Mesh::UpdateBuffer()
{
	ID3D11DeviceContext* pDeviceContext{ Locator::GetGraphicsService()->GetDeviceContext() };

	// ********************************
	// Update sub resource
	// ********************************
	/* CreateBuffer - Parameters */
	ID3D11Resource*  pDstResource{ m_pConstantBuffer };
	UINT			 DstSubresource{ 0 };
	const D3D11_BOX* pDstBox{ NULL };
	const void*		 pSrcData{ &m_InstancePositionData };
	UINT             SrcRowPitch{ 0 };
	UINT             SrcDepthPitch{ 0 };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-updatesubresource
	pDeviceContext->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}