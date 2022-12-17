#pragma once
#include "Camera.h"



struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Mesh;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

		void ToggleSamplerState();

	private:
		enum class SamplerState
		{
			Point = 0,
			Linear = 1,
			Anisotropic = 2
		};

		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		float m_AspectRatio{};

		bool m_IsInitialized{ false };

		

		//DIRECTX
		HRESULT InitializeDirectX();
		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		IDXGISwapChain* m_pSwapChain{};
		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};
		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};
		ID3D11SamplerState* m_pSamplerState{};

		SamplerState m_CurrentSamplerState{ SamplerState::Anisotropic };


		Mesh* m_pMesh;


		Camera m_Camera{};

		void CreateNewSamplerState(D3D11_FILTER newFiler);


	};
}
