#pragma once

namespace dae 
{
	class Texture;

	class Effect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~Effect();

		Effect(const Effect& other) = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect(Effect&& other) = delete;
		Effect& operator=(Effect&& other) = delete;

		ID3DX11Effect* GetEffectPtr() const;
		ID3DX11EffectTechnique* GetTechniquePtr() const;

		void UpdateWorldViewProjectionMatrix(const float* matrix);

		
		virtual void UpdateWorldMatrix(const float* matrix);
		virtual void UpdateViewInverseMatrix(const float* matrix);


	protected:
		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};

	private:
		ID3DX11EffectTechnique* m_pTechnique{};

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	};
}


