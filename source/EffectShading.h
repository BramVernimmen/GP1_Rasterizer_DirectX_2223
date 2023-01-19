#pragma once
#include "Effect.h"
namespace dae
{
    class Texture;

    class EffectShading : public Effect
    {
    public:
        EffectShading(ID3D11Device* pDevice, const std::wstring& assetFile);
        ~EffectShading();

        void UpdateWorldMatrix(const float* matrix);
        void UpdateViewInverseMatrix(const float* matrix);


        void SetDiffuseMap(Texture* pDiffuseTexture);
        void SetNormalMap(Texture* pNormalTexture);
        void SetSpecularMap(Texture* pSpecularTexture);
        void SetGlossinessMap(Texture* pGlossinessTexture);

    private:

        ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
        ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
        ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
        ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};


        ID3DX11EffectMatrixVariable* m_pWorldMatrix{};
        ID3DX11EffectMatrixVariable* m_pViewInverseMatrix{};
    };
}

