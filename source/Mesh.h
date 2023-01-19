#pragma once
namespace dae
{
	class Effect;
	class Texture;
	class Matrix;

	struct Vertex final
	{
		Vector3 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{}; //W3
		Vector3 normal{}; //W4
		Vector3 tangent{}; //W4
		//Vector3 viewDirection{}; //W4
	};

	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Effect* pEffect);
		~Mesh();

		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;

		void Update(const Timer* pTimer);

		void Render(ID3D11DeviceContext* pDeviceContext) const;
		void UpdateWorldViewProjectionMatrix(const Matrix& newMatrix, const Matrix& viewInverseMatrix) const;

		void UpdateSamplerState(ID3D11SamplerState* pNewSamplerState);

	private:
		const float m_RotationSpeed{ 45.0f * TO_RADIANS };
		Matrix m_WorldMatrix{};


		Effect* m_pEffect{};
		Texture* m_pDiffuseTexture{};
		Texture* m_pNormalTexture{};
		Texture* m_pSpecularTexture{};
		Texture* m_pGlossinessTexture{};

		ID3D11InputLayout* m_pInputLayout{};
		uint32_t m_NumIndices{};
		ID3D11Buffer* m_pVertexBuffer{};
		ID3D11Buffer* m_pIndexBuffer{};
	};
}


