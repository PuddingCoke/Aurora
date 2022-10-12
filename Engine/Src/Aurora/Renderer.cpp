#include<Aurora/Renderer.h>

Renderer* Renderer::instance = nullptr;

ID3D11Device4* Renderer::device = nullptr;

ID3D11DeviceContext4* Renderer::context = nullptr;

void Renderer::setViewport(const float& width, const float& height)
{
	instance->vp.Width = width;
	instance->vp.Height = height;
	context->RSSetViewports(1, &instance->vp);
}

void Renderer::setViewport(const unsigned int& width, const unsigned int& height)
{
	setViewport((float)width, (float)height);
}

void Renderer::setViewport(const int& width, const int& height)
{
	setViewport((float)width, (float)height);
}

void Renderer::setTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology)
{
	context->IASetPrimitiveTopology(topology);
}

void Renderer::setBlendState(ID3D11BlendState* const blendState)
{
	context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
}

void Renderer::setRasterizerState(ID3D11RasterizerState* const state)
{
	context->RSSetState(state);
}

void Renderer::drawQuad()
{
	context->Draw(3, 0);
}

void Renderer::drawCube()
{
	context->Draw(36, 0);
}

void Renderer::draw(const UINT& vertexCount, const UINT& startVertexLocation)
{
	context->Draw(vertexCount, startVertexLocation);
}

void Renderer::drawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation)
{
	context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void Renderer::drawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation)
{
	context->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

Renderer::Renderer(HWND hWnd, const unsigned int& width, const unsigned int& height, const bool& enableDebug, const unsigned int& msaaLevel) :
	vp{ 0.f,0.f,0.f,0.f,0.f,1.f }
{
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL maxSupportedFeatureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

	ComPtr<IDXGIDevice4> dxgiDevice;
	ComPtr<IDXGIAdapter3> dxgiAdapter;
	ComPtr<IDXGIFactory5> dxgiFactory;

	{
		ComPtr<ID3D11Device> device11;
		ComPtr<ID3D11DeviceContext> context11;

		D3D11_CREATE_DEVICE_FLAG deviceFlag = D3D11_CREATE_DEVICE_SINGLETHREADED;

		if (enableDebug)
		{
			std::cout << "[class Renderer] enable debug!\n";
			deviceFlag = (D3D11_CREATE_DEVICE_FLAG)((int)deviceFlag | (int)D3D11_CREATE_DEVICE_DEBUG);
		}
		else
		{
			std::cout << "[class Renderer] disable debug!\n";
		}

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlag, featureLevels, 2u,
			D3D11_SDK_VERSION, device11.GetAddressOf(), &maxSupportedFeatureLevel, context11.GetAddressOf());

		device11.As(&device4);
		context11.As(&context4);

		device = device4.Get();
		context = context4.Get();

		ComPtr<IDXGIDevice> dxgiDevice11;
		device11.As(&dxgiDevice11);

		ComPtr<IDXGIAdapter> dxgiAdapter11;
		dxgiDevice11->GetAdapter(dxgiAdapter11.ReleaseAndGetAddressOf());

		ComPtr<IDXGIFactory1> dxgiFactory11;
		dxgiAdapter11->GetParent(IID_IDXGIFactory1, (void**)dxgiFactory11.ReleaseAndGetAddressOf());

		dxgiDevice11.As(&dxgiDevice);
		dxgiAdapter11.As(&dxgiAdapter);
		dxgiFactory11.As(&dxgiFactory);
	}

	{
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 3;
		sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;

		ComPtr<IDXGISwapChain1> sc;
		dxgiFactory->CreateSwapChainForHwnd(Renderer::device, hWnd, &sd, nullptr, nullptr, sc.GetAddressOf());
		sc.As(&swapChain);
	}

	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

	swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)backBuffer.ReleaseAndGetAddressOf());

	if (msaaLevel == 1)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateRenderTargetView(backBuffer.Get(), &rtvDesc, defaultTargetView.ReleaseAndGetAddressOf());
	}
	else
	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = width;
		tDesc.Height = height;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		tDesc.SampleDesc.Count = msaaLevel;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		tDesc.CPUAccessFlags = 0;
		tDesc.MiscFlags = 0;

		Renderer::device->CreateTexture2D(&tDesc, nullptr, msaaTexture.ReleaseAndGetAddressOf());

		D3D11_RENDER_TARGET_VIEW_DESC msaaViewDesc = {};
		msaaViewDesc.Format = tDesc.Format;
		msaaViewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DMS;
		msaaViewDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateRenderTargetView(msaaTexture.Get(), &msaaViewDesc, Renderer::defaultTargetView.ReleaseAndGetAddressOf());
	}


	if (enableDebug)
	{
		Renderer::device->QueryInterface(IID_ID3D11Debug, (void**)d3dDebug.ReleaseAndGetAddressOf());
	}
}

void Renderer::setDefRTV(ID3D11DepthStencilView* const view)
{
	context->OMSetRenderTargets(1, instance->defaultTargetView.GetAddressOf(), view);
}

void Renderer::clearDefRTV(const float* color)
{
	context->ClearRenderTargetView(instance->defaultTargetView.Get(), color);
}