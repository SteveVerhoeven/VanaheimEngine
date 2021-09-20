#pragma once
#include <vector>

class Effect;
class TriangleMesh final
{
	public:
		enum class Cullmode
		{
			D3D11_CULL_NONE = 0,
			D3D11_CULL_FRONT = 1,
			D3D11_CULL_BACK = 2
		};

		TriangleMesh();
		~TriangleMesh();

		TriangleMesh(const TriangleMesh&) = delete;
		TriangleMesh& operator=(const TriangleMesh&) = delete;
		TriangleMesh(TriangleMesh&&) = delete;
		TriangleMesh& operator=(TriangleMesh&&) = delete;

		void InitializeMesh(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices);
		void Update();
		void Render(ID3D11DeviceContext* pDeviceContext);

		void AddEffect(ID3D11Device* pDevice, const std::string& effectFilePath);

	private:
		Cullmode m_Cullmode;

		uint32_t m_AmountIndices;
		ID3D11Buffer* m_pIBuffer;
		ID3D11Buffer* m_pVBuffer;
		ID3D11InputLayout* m_pVertexLayout;

		Effect* m_pEffect;

		std::vector<Vertex_Input> m_VBuffer;
		std::vector<uint32_t> m_IBuffer;

		HRESULT CreateVertex_InputLayout(ID3D11Device* pDevice);
		HRESULT CreateIndexBuffer(ID3D11Device* pDevice, const std::vector<uint32_t>& iBuffer);
		HRESULT CreateVertexBuffer(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vBuffer);

		void SetFilterMethod();
		void SetRasterizerState(ID3D11DeviceContext* pDeviceContext);
		void SetBlendingState(ID3D11DeviceContext* pDeviceContext);
		void SetVertexBuffers(ID3D11DeviceContext* pDeviceContext);
		void SetIndexBuffers(ID3D11DeviceContext* pDeviceContext);
		void SetInputLayout(ID3D11DeviceContext* pDeviceContext);
		void SetPrimTopology(ID3D11DeviceContext* pDeviceContext);
		void RenderTriangle(ID3D11DeviceContext* pDeviceContext);
};