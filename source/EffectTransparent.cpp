#include "pch.h"
#include "EffectTransparent.h"
#include "Texture.h"

namespace dae
{

	EffectTransparent::EffectTransparent(ID3D11Device* pDevice, const std::wstring& assetFile)
		: Effect(pDevice, assetFile)
	{
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
			std::wcout << L"m_pDiffuseMapVariable not valid\n";
	}
	
	EffectTransparent::~EffectTransparent()
	{
		m_pDiffuseMapVariable->Release();
	}
	
	void EffectTransparent::SetDiffuseMap(Texture* pTexture)
	{
		m_pDiffuseMapVariable->SetResource(pTexture->GetSRV());
	}

}