#include "pch.h"
#include "TriangleMesh.h"

#include "Effect.h"

TriangleMesh::TriangleMesh()
	        : m_Cullmode( Cullmode::D3D11_CULL_BACK )
	        , m_AmountIndices( 0 )
	        , m_pIBuffer( nullptr )
	        , m_pVBuffer( nullptr )
	        , m_pVertexLayout( nullptr )
	        , m_pEffect( nullptr )
	        , m_VBuffer( std::vector<Vertex_Input>() )
	        , m_IBuffer( std::vector<uint32_t>() )
{}
TriangleMesh::~TriangleMesh()
{
	DELETE_POINTER(m_pEffect);

	DELETE_RESOURCE(m_pIBuffer);
	DELETE_RESOURCE(m_pVBuffer);

	DELETE_RESOURCE(m_pVertexLayout);

	m_IBuffer.clear();
	m_VBuffer.clear();
}

void TriangleMesh::InitializeMesh(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices)
{
	HRESULT hr{};
	// ********************************
	// Create vertex buffer
	// ********************************
	hr = CreateVertexBuffer(pDevice, vertices);
	if (FAILED(hr))
		return;

	// ********************************
	// Create vertex & input layout
	// ********************************
	hr = CreateVertex_InputLayout(pDevice);
	if (FAILED(hr))
		return;

	// ********************************
	// Create Index Buffer
	// ********************************
	hr = CreateIndexBuffer(pDevice, indices);
	if (FAILED(hr))
		return;
}
void TriangleMesh::Update()
{
	
}

void TriangleMesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	SetBlendingState(pDeviceContext);
	SetRasterizerState(pDeviceContext);
	SetFilterMethod();

	SetVertexBuffers(pDeviceContext);
	SetIndexBuffers(pDeviceContext);
	SetInputLayout(pDeviceContext);
	SetPrimTopology(pDeviceContext);
	RenderTriangle(pDeviceContext);
}

void TriangleMesh::AddEffect(ID3D11Device* pDevice, const std::string& effectFilePath)
{
	m_pEffect = new Effect();
	m_pEffect->Initialize(pDevice, effectFilePath);
}

HRESULT TriangleMesh::CreateVertex_InputLayout(ID3D11Device* pDevice)
{
	// ********************************
	// Create vertex input layout
	// ********************************
	static const uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	/* AlignedByteOffset is 1 long buffer so ... */
	// 'Position'	is an FPoint3 which is made of 3 floats so it takes 12 bytes of the that buffer
	// 'Color'		is an RGB of 3 floats so 3x4 is 12 Byte of that buffer

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//vertexDesc[2].SemanticName = "TEXCOORD";
	//vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	//vertexDesc[2].AlignedByteOffset = 24;
	//vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//vertexDesc[3].SemanticName = "NORMAL";
	//vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//vertexDesc[3].AlignedByteOffset = 32;
	//vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//vertexDesc[4].SemanticName = "TANGENT";
	//vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//vertexDesc[4].AlignedByteOffset = 44;
	//vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//vertexDesc[5].SemanticName = "WORLDPOSITION";
	//vertexDesc[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//vertexDesc[5].AlignedByteOffset = 56;
	//vertexDesc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// ********************************
	// Create Input Layout
	// ********************************
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	//if (m_EffectType == Effect::EffectType::LAMBERT)
	//{
	//	ID3DX11EffectTechnique* t = m_pEffect_Lambert->GetTechnique();
	//	auto pass = t->GetPassByIndex(0);
	//	pass->GetDesc(&passDesc);
	//}
	//else if (m_EffectType == Effect::EffectType::LAMBERT_PHONG)
	//	m_pEffect_LambertPhong->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

	/* CreateInputLayout - Parameters */
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs{ vertexDesc };
	UINT nbrElements{ numElements };
	const void* pShaderBytecodeWithInputSignature{ passDesc.pIAInputSignature };
	size_t bytecodeLength{ passDesc.IAInputSignatureSize };
	ID3D11InputLayout** ppInputLayout{ &m_pVertexLayout };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createinputlayout
	return pDevice->CreateInputLayout(pInputElementDescs, nbrElements, pShaderBytecodeWithInputSignature, bytecodeLength, ppInputLayout);
	//return HRESULT();
}
HRESULT TriangleMesh::CreateIndexBuffer(ID3D11Device* pDevice, const std::vector<uint32_t>& iBuffer)
{
	// ********************************
	// Create Index Buffer
	// ********************************
	m_AmountIndices = (uint32_t)iBuffer.size();
	D3D11_BUFFER_DESC ibd{};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData_IBuffer{ 0 };
	initData_IBuffer.pSysMem = iBuffer.data();

	/* CreateBuffer - Parameters */
	const D3D11_BUFFER_DESC* pIBufferDesc{ &ibd };
	const D3D11_SUBRESOURCE_DATA* pInitialData_IBuffer{ &initData_IBuffer };
	ID3D11Buffer** ppIBuffer{ &m_pIBuffer };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer
	return pDevice->CreateBuffer(pIBufferDesc, pInitialData_IBuffer, ppIBuffer);
}
HRESULT TriangleMesh::CreateVertexBuffer(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vBuffer)
{
	// ********************************
	// Create vertex buffer
	// ********************************
	D3D11_BUFFER_DESC vbd{};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex_Input) * (uint32_t)vBuffer.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData_VBuffer{ 0 };
	initData_VBuffer.pSysMem = vBuffer.data();

	/* CreateBuffer - Parameters */
	const D3D11_BUFFER_DESC* pVBufferDesc{ &vbd };
	const D3D11_SUBRESOURCE_DATA* pInitialData_VBuffer{ &initData_VBuffer };
	ID3D11Buffer** ppVBuffer{ &m_pVBuffer };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-createbuffer
	return pDevice->CreateBuffer(pVBufferDesc, pInitialData_VBuffer, ppVBuffer);
}

void TriangleMesh::SetFilterMethod()
{
	ID3DX11EffectSamplerVariable* samplerVariable{ m_pEffect->GetSamplerVariable() };
	Effect::SamplingType samplingState{ m_pEffect->GetSamplingType() };

	if (samplingState == Effect::SamplingType::POINT)
		samplerVariable->SetSampler(0, m_pEffect->GetSamplerPoint());
	else if (samplingState == Effect::SamplingType::LINEAR)
		samplerVariable->SetSampler(0, m_pEffect->GetSamplerLinear());
	else if (samplingState == Effect::SamplingType::ANISOTROPIC)
		samplerVariable->SetSampler(0, m_pEffect->GetSamplerAnisotropic());
}
void TriangleMesh::SetRasterizerState(ID3D11DeviceContext* pDeviceContext)
{
	UNREFERENCED_PARAMETER(pDeviceContext);

	//if (m_pEffect_LambertPhong)
	//{
	//	/* RSSetState - Parameters */
	//	// Explanation for all parameters in link below
	//	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-rssetstate
	//	if (m_Cullmode == Cullmode::D3D11_CULL_BACK)
	//		pDeviceContext->RSSetState(m_pEffect_LambertPhong->GetRasterizerState_BACK());
	//	else if (m_Cullmode == Cullmode::D3D11_CULL_FRONT)
	//		pDeviceContext->RSSetState(m_pEffect_LambertPhong->GetRasterizerState_FRONT());
	//	else if (m_Cullmode == Cullmode::D3D11_CULL_NONE)
	//		pDeviceContext->RSSetState(m_pEffect_LambertPhong->GetRasterizerState_NONE());
	//}
}
void TriangleMesh::SetBlendingState(ID3D11DeviceContext* pDeviceContext)
{
	UNREFERENCED_PARAMETER(pDeviceContext);
	//if (m_pEffect_Lambert)
	//{
	//	/* CreateBlendState - Parameters */
	//	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//	UINT sampleMask = 0xffffffff;
	//	// Explanation for all parameters in link below
	//	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-omsetblendstate
	//	if (!m_Transparancy_ON)
	//		pDeviceContext->OMSetBlendState(m_pEffect_Lambert->GetBlendState_OFF(), blendFactor, sampleMask);
	//	else
	//		pDeviceContext->OMSetBlendState(m_pEffect_Lambert->GetBlendState_ON(), blendFactor, sampleMask);
	//}
}
void TriangleMesh::SetVertexBuffers(ID3D11DeviceContext* pDeviceContext)
{
	// ********************************
	// Set vertex buffer
	// ********************************
	UINT stride{ sizeof(Vertex_Input) };
	UINT offset1{ 0 };
	/* IASetVertexBuffers - Parameters */
	UINT startSlot{ 0 };
	UINT numBuffers{ 1 };
	ID3D11Buffer* const* ppVertexBuffers{ &m_pVBuffer };
	const UINT* pStrides{ &stride };
	const UINT* pOffsets{ &offset1 };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetvertexbuffers
	pDeviceContext->IASetVertexBuffers(startSlot, numBuffers, ppVertexBuffers, pStrides, pOffsets);
}
void TriangleMesh::SetIndexBuffers(ID3D11DeviceContext* pDeviceContext)
{
	// ********************************
	// Set index buffer
	// ********************************
	/* IASetIndexBuffer - Parameters */
	ID3D11Buffer* pIndexBuffer{ m_pIBuffer };
	DXGI_FORMAT format{ DXGI_FORMAT_R32_UINT };
	UINT offset2{ 0 };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetindexbuffer
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, format, offset2);
}
void TriangleMesh::SetInputLayout(ID3D11DeviceContext* pDeviceContext)
{
	// ********************************
	// Set the input layout
	// ********************************
	/* IASetInputLayout - Parameters */
	ID3D11InputLayout* pInputLayout{ m_pVertexLayout };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetinputlayout
	pDeviceContext->IASetInputLayout(pInputLayout);
}
void TriangleMesh::SetPrimTopology(ID3D11DeviceContext* pDeviceContext)
{
	// ********************************
	// Set primitive topology
	// ********************************
	/* IASetPrimitiveTopology - Parameters */
	D3D11_PRIMITIVE_TOPOLOGY topology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetvertexbuffers
	pDeviceContext->IASetPrimitiveTopology(topology);
}
void TriangleMesh::RenderTriangle(ID3D11DeviceContext* pDeviceContext)
{
	// ********************************
	// Render a triangle
	// ********************************
	ID3DX11EffectTechnique* pTechnique{ m_pEffect->GetTechnique() };
	
	D3DX11_TECHNIQUE_DESC techDesc{};
	pTechnique->GetDesc(&techDesc);
	for (UINT p{}; p < techDesc.Passes; ++p)
	{
		pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
	}
}