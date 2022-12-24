#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Utils.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		m_AspectRatio = m_Width / static_cast<float>(m_Height);


		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}


		//m_Camera.Initialize(45.f, { 0.0f, 0.0f, -10.0f }, m_AspectRatio);
		m_Camera.Initialize(45.f, { 0.0f, 0.0f, -50.0f }, m_AspectRatio);



		//Create some data for our mesh
		// pos in NDC
		//std::vector<Vertex> vertices{
		//	{{0.0f, 0.5f, 0.5f},	{1.f, 0.f, 0.f}},
		//	{{0.5f, -0.5f, 0.5f},	{0.f, 0.f, 1.f}},
		//	{{-0.5f, -0.5f, 0.5f},	{0.f, 1.f, 0.f}},
		//};
		
		// pos in WORLD -- Triangle
		//std::vector<Vertex> vertices{
		//	{{0.0f, 3.f, 2.f},	{1.f, 0.f, 0.f}},
		//	{{3.f, -3.f, 2.f},	{0.f, 0.f, 1.f}},
		//	{{-3.f, -3.f, 2.f},	{0.f, 1.f, 0.f}},
		//};

		//std::vector<Vertex> vertices{
		//	{
		//		Vertex{{-3, 3, -2},		{1,1,1},	{0,0}},
		//		Vertex{{0,3,-2},		{1,1,1},	{0.5f, 0}},
		//		Vertex{{3,3,-2},		{1,1,1},	{1,0}},
		//		Vertex{{-3, 0, -2},		{1,1,1},	{0, 0.5f}},
		//		Vertex{{0,0,-2},		{1,1,1},	{0.5f, 0.5f}},
		//		Vertex{{3,0,-2},		{1,1,1},	{1, 0.5f}},
		//		Vertex{{-3,-3,-2},		{1,1,1},	{0,1}},
		//		Vertex{{0,-3,-2},		{1,1,1},	{0.5f, 1}},
		//		Vertex{{3,-3,-2},		{1,1,1},	{1,1}}
		//	}
		//};



		//std::vector<uint32_t> indices{ 0,1,2 };
		//std::vector<uint32_t> indices{ 
		//		3,0,1,		1,4,3,		4,1,2,
		//		2,5,4,		6,3,4,		4,7,6,
		//		7,4,5,		5,8,7 
		//};

		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		if (Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices) == false)
			std::wcout << L"ParseOBJ FAILED\n";

		m_pMesh = new Mesh(m_pDevice, vertices, indices);


		//CreateNewSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT);
		ToggleSamplerState(); // -> this will print the current state
	}

	Renderer::~Renderer()
	{
		m_pSamplerState->Release();

		delete m_pMesh;
		m_pMesh = nullptr;

		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();
		m_pDepthStencilView->Release();
		m_pDepthStencilBuffer->Release();
		m_pSwapChain->Release();
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		m_pDevice->Release();

	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_Camera.Update(pTimer);
		m_pMesh->Update(pTimer);
		m_pMesh->UpdateWorldViewProjectionMatrix(m_Camera.GetViewMatrix() * m_Camera.GetProjectionMatrix());
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;


		//1. CLEAR RTV & DSV
		ColorRGB clearColor = ColorRGB{ 0.f, 0.f, 0.3f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//2. SET PIPELINE + INVOKE DRAWCALLS (= RENDER)
		//...



		//3. PRESENT BACKBUFFER (SWAP)
		m_pMesh->Render(m_pDeviceContext);
		m_pSwapChain->Present(0, 0);



	}



	HRESULT Renderer::InitializeDirectX()
	{
		// 1. Create Device & DeviceContext
		//=====
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 
			1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
			return result;


		// Create DXGI Factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;


		//2. Create Swapchain
		//=====
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect - DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		//Get the handle (HWND) from the SDL Backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		//Create SwapChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;


		pDxgiFactory->Release();


		//3. Create DepthStencil (DS) & DepthStencilView (DSV)
		//Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;


		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;




		//4. Create RenderTarget (RT) & RenderTargetView (RTV)
		//=====

		//Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;


		//5. Bind RTV & DSV to Output Merger Stage
		//=====
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);



		//6. Set Viewport
		//=====
		D3D11_VIEWPORT viewPort{};
		viewPort.Width = static_cast<float>(m_Width);
		viewPort.Height = static_cast<float>(m_Height);
		viewPort.TopLeftX = 0.f;
		viewPort.TopLeftY = 0.f;
		viewPort.MinDepth = 0.f;
		viewPort.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewPort);

		

		return result;
		//return S_FALSE;
	}

	void Renderer::ToggleSamplerState()
	{
		m_CurrentSamplerState = SamplerState((static_cast<int>(m_CurrentSamplerState) + 1) % 3);

		switch (m_CurrentSamplerState)
		{
		case dae::Renderer::SamplerState::Point:
			CreateNewSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT);
			std::wcout << "\033[33m" << L"SAMPLER STATE: POINT\n" << "\033[0m";
			break;
		case dae::Renderer::SamplerState::Linear:
			CreateNewSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
			std::wcout << "\033[31m" << L"SAMPLER STATE: LINEAR\n" << "\033[0m";
			break;
		case dae::Renderer::SamplerState::Anisotropic:
			CreateNewSamplerState(D3D11_FILTER_ANISOTROPIC);
			std::wcout << "\033[32m" << L"SAMPLER STATE: ANISOTROPIC\n" << "\033[0m";
			break;
		}
	}



	void Renderer::CreateNewSamplerState(D3D11_FILTER newFiler)
	{
		// release the old one
		if (m_pSamplerState)
			m_pSamplerState->Release();

		D3D11_SAMPLER_DESC newSamplerDesc{}; // info -> https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_sampler_desc
		newSamplerDesc.Filter = newFiler;
		newSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		newSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		newSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		newSamplerDesc.MinLOD = (-FLT_MAX);
		newSamplerDesc.MaxLOD = FLT_MAX;
		newSamplerDesc.MipLODBias = 0.0f;
		newSamplerDesc.MaxAnisotropy = 1;
		newSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		//newSamplerDesc.BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

		HRESULT result = m_pDevice->CreateSamplerState(&newSamplerDesc, &m_pSamplerState);
		if (FAILED(result))
			std::wcout << L"m_pSamplerState not correct\n";

		m_pMesh->UpdateSamplerState(m_pSamplerState);
	}
}
