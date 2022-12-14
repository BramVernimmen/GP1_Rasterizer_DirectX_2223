#pragma once

namespace dae 
{
	class Texture;

	class Effect final
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~Effect();

		Effect(const Effect& other) = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect(Effect&& other) = delete;
		Effect& operator=(Effect&& other) = delete;

		ID3DX11Effect* GetEffectPtr() const;
		ID3DX11EffectTechnique* GetTechniquePtr() const;

		void UpdateWorldViewProjectionMatrix(const float* matrix);

		void SetDiffuseMap(Texture* pDiffuseTexture);

	private:
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};


		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	};
}


