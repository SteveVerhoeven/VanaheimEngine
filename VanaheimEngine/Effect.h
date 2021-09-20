#pragma once
#include <vector>

class GameObject;
class Effect
{
	public:
		enum class EffectType
		{
			BASIC = 0,
			LAMBERT = 1,
			LAMBERT_PHONG = 2
		};
		enum class SamplingType
		{
			POINT = 0,
			LINEAR = 1,
			ANISOTROPIC = 2
		};

		Effect();
		virtual ~Effect();

		Effect(const Effect&) = delete;
		Effect& operator=(const Effect&) = delete;
		Effect(Effect&&) = delete;
		Effect& operator=(Effect&&) = delete;

		void Initialize(ID3D11Device* pDevice, const std::string& assetFile);
		void Update(GameObject* pParentGameObject);

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::string& effectFilePath);

		ID3DX11Effect* GetEffect() const { return m_pDirectXEffect; }
		ID3DX11EffectTechnique* GetTechnique() const { return m_pDefaultTechnique; }

		const SamplingType& GetSamplingType() const { return m_SamplingState; }
		ID3DX11EffectSamplerVariable* GetSamplerVariable() const { return m_pSamplerVariable; }
		virtual ID3D11SamplerState* GetSamplerPoint() { return m_pSamplerState_Point; }
		virtual ID3D11SamplerState* GetSamplerLinear() { return m_pSamplerState_Linear; }
		virtual ID3D11SamplerState* GetSamplerAnisotropic() { return m_pSamplerState_Anisotropic; }

	protected:
		SamplingType m_SamplingState;
		ID3DX11EffectSamplerVariable* m_pSamplerVariable;
		ID3D11SamplerState* m_pSamplerState_Point;
		ID3D11SamplerState* m_pSamplerState_Linear;
		ID3D11SamplerState* m_pSamplerState_Anisotropic;

		ID3DX11Effect* m_pDirectXEffect;
		ID3DX11EffectTechnique* m_pDefaultTechnique;
		ID3DX11EffectMatrixVariable* m_pMat_WorldViewProjVariable;

		ID3DX11EffectTechnique* BindTechniqueVariable(const std::string& techniqueName);
		ID3DX11EffectMatrixVariable* BindMatrixVariable(const std::string& matrixName);
		ID3DX11EffectShaderResourceVariable* BindTextureVariable(const std::string& textureName);
		ID3DX11EffectRasterizerVariable* BindRasterizerStateVariable(const std::string& textureName);

	private:
		void CreateSamplerStates(ID3D11Device* pDevice, const D3D11_FILTER& filter);
};