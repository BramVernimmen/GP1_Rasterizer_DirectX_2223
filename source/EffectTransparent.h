#pragma once
#include "Effect.h"
namespace dae 
{
	class Texture;

	class EffectTransparent final : public Effect
	{
	public:
		EffectTransparent(ID3D11Device* pDevice, const std::wstring& assetFile);
		~EffectTransparent();

		void SetDiffuseMap(Texture* pTexture);

	private:
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
	};

}
