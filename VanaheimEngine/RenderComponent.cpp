#include "pch.h"
#include "RenderComponent.h"

#include "Mesh.h"
#include "Material.h"

RenderComponent::RenderComponent()
				: Component()
				, m_CanRenderComponent(false)
{}

void RenderComponent::Initialize(Scene* /*pParentScene*/)
{}
void RenderComponent::PostInitialize(Scene* /*pParentScene*/)
{}
void RenderComponent::Update(const float /*elapsedSec*/)
{}
void RenderComponent::FixedUpdate(const float /*timeEachUpdate*/)
{}

void RenderComponent::Render3DMesh(ID3D11DeviceContext* pDeviceContext, Mesh* p3DMesh, Material* pEffect)
{
	SetBlendingState(pDeviceContext);
	SetRasterizerState(pDeviceContext);
	SetFilterMethod(pEffect);
	SetVertexBuffers(pDeviceContext, p3DMesh);
	SetIndexBuffers(pDeviceContext, p3DMesh);
	SetInstanceBuffers(pDeviceContext, p3DMesh);
	SetInputLayout(pDeviceContext, p3DMesh);
	SetPrimTopology(pDeviceContext);
	Render(pDeviceContext, pEffect, p3DMesh);
}

void RenderComponent::SetBlendingState(ID3D11DeviceContext* pDeviceContext)
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
void RenderComponent::SetRasterizerState(ID3D11DeviceContext* pDeviceContext)
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
void RenderComponent::SetFilterMethod(Material* pEffect)
{
	ID3DX11EffectSamplerVariable* samplerVariable{ pEffect->GetSamplerVariable() };
	Material::SamplingType samplingState{ pEffect->GetSamplingType() };

	if (samplingState == Material::SamplingType::POINT)
		samplerVariable->SetSampler(0, pEffect->GetSamplerPoint());
	else if (samplingState == Material::SamplingType::LINEAR)
		samplerVariable->SetSampler(0, pEffect->GetSamplerLinear());
	else if (samplingState == Material::SamplingType::ANISOTROPIC)
		samplerVariable->SetSampler(0, pEffect->GetSamplerAnisotropic());
}
void RenderComponent::SetVertexBuffers(ID3D11DeviceContext* pDeviceContext, Mesh* p3DMesh)
{
	if (p3DMesh->GetAmountInstances() > 0)
		SetVertexBuffersInstanced(pDeviceContext, p3DMesh);
	else
		SetVertexBuffersNormal(pDeviceContext, p3DMesh);
}
void RenderComponent::SetIndexBuffers(ID3D11DeviceContext* pDeviceContext, Mesh* p3DMesh)
{
	ID3D11Buffer* iBuffer{ p3DMesh->GetIBuffer() };
	// ********************************
	// Set index buffer
	// ********************************
	/* IASetIndexBuffer - Parameters */
	ID3D11Buffer* pIndexBuffer{ iBuffer };
	DXGI_FORMAT format{ DXGI_FORMAT_R32_UINT };
	UINT offset{ 0 };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetindexbuffer
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, format, offset);
}
void RenderComponent::SetInstanceBuffers(ID3D11DeviceContext* pDeviceContext, Mesh* p3DMesh)
{
	if (p3DMesh->GetAmountInstances() == 0)
		return;

	ID3D11Buffer* pConstantBuffer = p3DMesh->GetConstantBuffer();
	// ********************************
	// Set constant buffers
	// ********************************
	/* IASetIndexBuffer - Parameters */
	UINT                 StartSlot{};
	UINT                 NumBuffers{ 1 };
	ID3D11Buffer* const* ppConstantBuffers{ &pConstantBuffer };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-vssetconstantbuffers
	pDeviceContext->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}
void RenderComponent::SetInputLayout(ID3D11DeviceContext* pDeviceContext, Mesh* p3DMesh)
{
	// ********************************
	// Set the input layout
	// ********************************
	/* IASetInputLayout - Parameters */
	ID3D11InputLayout* pInputLayout{ p3DMesh->GetInputLayout() };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetinputlayout
	pDeviceContext->IASetInputLayout(pInputLayout);
}
void RenderComponent::SetPrimTopology(ID3D11DeviceContext* pDeviceContext)
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
void RenderComponent::Render(ID3D11DeviceContext* pDeviceContext, Material* pEffect, Mesh* p3DMesh)
{
	if (p3DMesh->GetAmountInstances() > 0)
		RenderInstanced(pDeviceContext, pEffect, p3DMesh);
	else
		RenderNormal(pDeviceContext, pEffect, p3DMesh);
}

void RenderComponent::SetVertexBuffersNormal(ID3D11DeviceContext* pDeviceContext, Mesh* p3DMesh)
{
	ID3D11Buffer* vBuffer{ p3DMesh->GetVBuffer() };
	// ********************************
	// Set vertex buffer
	// ********************************
	UINT stride{ sizeof(Vertex_Input) };
	UINT offset1{ 0 };
	/* IASetVertexBuffers - Parameters */
	UINT startSlot{ 0 };
	UINT numBuffers{ 1 };
	ID3D11Buffer* const* ppVertexBuffers{ &vBuffer };
	const UINT* pStrides{ &stride };
	const UINT* pOffsets{ &offset1 };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetvertexbuffers
	pDeviceContext->IASetVertexBuffers(startSlot, numBuffers, ppVertexBuffers, pStrides, pOffsets);
}
void RenderComponent::SetVertexBuffersInstanced(ID3D11DeviceContext* pDeviceContext, Mesh* p3DMesh)
{
	ID3D11Buffer* vBuffer{ p3DMesh->GetVBuffer() };
	ID3D11Buffer* instBuffer{ p3DMesh->GetInstanceBuffer() };
	ID3D11Buffer* vertInstBuffers[2] = { vBuffer, instBuffer };
	// ********************************
	// Set vertex buffer
	// ********************************
	UINT strides[2] = { sizeof(Vertex_Input), sizeof(DirectX::XMFLOAT3) };
	UINT offsets[2] = { 0, 0 };
	/* IASetVertexBuffers - Parameters */
	UINT startSlot{ 0 };
	UINT numBuffers{ 2 };
	ID3D11Buffer* const* ppVertexBuffers{ vertInstBuffers };
	const UINT* pStrides{ strides };
	const UINT* pOffsets{ offsets };

	// Explanation for all parameters in link below
	// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetvertexbuffers
	pDeviceContext->IASetVertexBuffers(startSlot, numBuffers, ppVertexBuffers, pStrides, pOffsets);
}
void RenderComponent::RenderNormal(ID3D11DeviceContext* pDeviceContext, Material* pEffect, Mesh* p3DMesh)
{
	ID3DX11EffectTechnique* pTechnique{ pEffect->GetTechnique() };

	D3DX11_TECHNIQUE_DESC techDesc{};
	pTechnique->GetDesc(&techDesc);
	for (UINT p{}; p < techDesc.Passes; ++p)
	{
		// COMMENT HERE
		pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);

		// ****************************************
		// Set up the DrawIndexed function
		// ****************************************
		/* DrawIndexed - Parameters */
		UINT IndexCount{ p3DMesh->GetAmountIndices() };
		UINT StartIndexLocation{ 0 };
		INT  BaseVertexLocation{ 0 };

		// Explanation for all parameters in link below
		// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-drawindexed
		pDeviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	}
}
void RenderComponent::RenderInstanced(ID3D11DeviceContext* pDeviceContext, Material* pEffect, Mesh* p3DMesh)
{
	if (p3DMesh->GetIsRendered())
		return;

	ID3DX11EffectTechnique* pTechnique{ pEffect->GetTechnique() };

	D3DX11_TECHNIQUE_DESC techDesc{};
	pTechnique->GetDesc(&techDesc);
	for (UINT p{}; p < techDesc.Passes; ++p)
	{
		// COMMENT HERE
		pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);

		// ****************************************
		// Set up the DrawIndexedInstanced function
		// ****************************************
		/* DrawIndexedInstanced - Parameters */
		UINT IndexCountPerInstance{ p3DMesh->GetAmountIndices() };
		UINT InstanceCount{ p3DMesh->GetAmountInstances() };
		UINT StartIndexLocation{ 0 };
		INT  BaseVertexLocation{ 0 };
		UINT StartInstanceLocation{ 0 };

		// Explanation for all parameters in link below
		// Reference: https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-drawindexedinstanced
		pDeviceContext->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
	}

	p3DMesh->SetIsRendered(true);
}