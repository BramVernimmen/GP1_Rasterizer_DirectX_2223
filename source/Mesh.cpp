#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
#include "Texture.h"
#include "Matrix.h"

namespace dae
{
	Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		: m_pEffect{new Effect(pDevice, L"Resources/PosCol3D.fx")}
	{
		//m_pTexture = Texture::LoadFromFile(pDevice, "Resources/uv_grid_2.png");
		m_pTexture = Texture::LoadFromFile(pDevice, "Resources/vehicle_diffuse.png");
		m_pEffect->SetDiffuseMap(m_pTexture);


		//Create Vertex Layout
		static constexpr uint32_t numElements{ 3 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};
		
		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "TEXCOORD";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[2].AlignedByteOffset = 24;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		//Create Input Layout
		D3DX11_PASS_DESC passDesc{};
		m_pEffect->GetTechniquePtr()->GetPassByIndex(0)->GetDesc(&passDesc);

		HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
			numElements,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_pInputLayout);

		if (FAILED(result))
			return; // or assert(false)

		// Create vertex buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = vertices.data();

		result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result))
			return;


		//Create index buffer
		m_NumIndices = static_cast<uint32_t>(indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = indices.data();
		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result))
			return;


	}

	Mesh::~Mesh()
	{
		if (m_pIndexBuffer)
			m_pIndexBuffer->Release();
		if (m_pVertexBuffer)
			m_pVertexBuffer->Release();
		if (m_pInputLayout)
			m_pInputLayout->Release();
		delete m_pTexture;
		delete m_pEffect;
	}

	void Mesh::Update(const Timer* pTimer)
	{
		m_WorldMatrix *= Matrix::CreateRotationY(m_RotationSpeed * pTimer->GetElapsed());
	}

	void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
	{
		//1. Set Primitive Topology
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//2. Set Inputt Layout
		pDeviceContext->IASetInputLayout(m_pInputLayout);

		//3. Set VertexBuffer
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		//4. Set IndexBuffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//5. Draw
		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechniquePtr()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffect->GetTechniquePtr()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}

	}
	void Mesh::UpdateWorldViewProjectionMatrix(const Matrix& newMatrix) const
	{
		// TODO ; don't forget mesh personal matrix (for translate/rotate)
		Matrix fullMatrix = m_WorldMatrix * newMatrix;
		

		m_pEffect->UpdateWorldViewProjectionMatrix(reinterpret_cast<const float*>(&fullMatrix));
	}

	void Mesh::UpdateSamplerState(ID3D11SamplerState* pNewSamplerState)
	{
		// temporarily store the Sampler Variable from Effect
		ID3DX11EffectSamplerVariable* pSamplerStateVariable{ m_pEffect->GetEffectPtr()->GetVariableByName("gSamplerState")->AsSampler() };
		// check if valid
		if (!pSamplerStateVariable->IsValid())
			std::wcout << L"pSamplerStateVariable not valid\n";

		HRESULT result = pSamplerStateVariable->SetSampler(0, pNewSamplerState);
		if (FAILED(result))
			std::wcout << L"pNewSamplerState not correct  (in Mesh class)\n";

	}
}
