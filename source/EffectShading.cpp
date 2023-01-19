#include "pch.h"
#include "EffectShading.h"
#include "Texture.h"

namespace dae
{
	EffectShading::EffectShading(ID3D11Device* pDevice, const std::wstring& assetFile)
		:Effect(pDevice, assetFile)
	{
		m_pWorldMatrix = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
		if (!m_pWorldMatrix->IsValid())
			std::wcout << L"m_pWorldMatrix not valid\n";

		m_pViewInverseMatrix = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
		if (!m_pViewInverseMatrix->IsValid())
			std::wcout << L"m_pViewInverseMatrix not valid\n";



		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
			std::wcout << L"m_pDiffuseMapVariable not valid\n";

		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid())
			std::wcout << L"m_pNormalMapVariable not valid\n";

		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid())
			std::wcout << L"m_pSpecularMapVariable not valid\n";

		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid())
			std::wcout << L"m_pGlossinessMapVariable not valid\n";
	}

	EffectShading::~EffectShading()
	{
		m_pDiffuseMapVariable->Release();
		m_pNormalMapVariable->Release();
		m_pSpecularMapVariable->Release();
		m_pGlossinessMapVariable->Release();

		m_pWorldMatrix->Release();
		m_pViewInverseMatrix->Release();
	}

	void EffectShading::UpdateWorldMatrix(const float* matrix)
	{
		m_pWorldMatrix->SetMatrix(matrix);
	}

	void EffectShading::UpdateViewInverseMatrix(const float* matrix)
	{
		m_pViewInverseMatrix->SetMatrix(matrix);
	}



	void EffectShading::SetDiffuseMap(Texture* pDiffuseTexture)
	{
		if (m_pDiffuseMapVariable)
			m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetSRV());
	}

	void EffectShading::SetNormalMap(Texture* pNormalTexture)
	{
		if (m_pNormalMapVariable)
			m_pNormalMapVariable->SetResource(pNormalTexture->GetSRV());
	}

	void EffectShading::SetSpecularMap(Texture* pSpecularTexture)
	{
		if (m_pSpecularMapVariable)
			m_pSpecularMapVariable->SetResource(pSpecularTexture->GetSRV());
	}

	void EffectShading::SetGlossinessMap(Texture* pGlossinessTexture)
	{
		if (m_pGlossinessMapVariable)
			m_pGlossinessMapVariable->SetResource(pGlossinessTexture->GetSRV());
	}
}
