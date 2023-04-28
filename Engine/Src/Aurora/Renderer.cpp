#include<Aurora/Renderer.h>

Renderer* Renderer::instance = nullptr;

ID3D11Device5* Renderer::getDevice()
{
	return instance->device5.Get();
}

ID3D11DeviceContext4* Renderer::getContext()
{
	return instance->context4.Get();
}

const GPUManufacturer& Renderer::getGPUManufacturer()
{
	return instance->gpuManufacturer;
}

Renderer::Renderer(HWND hWnd, const unsigned int& width, const unsigned int& height, const bool& enableDebug, const unsigned int& msaaLevel, const UINT& extraDeviceFlags) :
	vp{ 0.f,0.f,0.f,0.f,0.f,1.f }
{
	instance = this;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL maxSupportedFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	ComPtr<IDXGIDevice4> dxgiDevice;
	ComPtr<IDXGIAdapter3> dxgiAdapter;
	ComPtr<IDXGIFactory5> dxgiFactory;

	{
		ComPtr<ID3D11Device> device11;
		ComPtr<ID3D11DeviceContext> context11;

		UINT deviceFlag = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT | extraDeviceFlags;

		if (enableDebug)
		{
			std::cout << "[class Renderer] enable debug!\n";
			deviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
		}
		else
		{
			std::cout << "[class Renderer] disable debug!\n";
		}

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlag, featureLevels, ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION, device11.ReleaseAndGetAddressOf(), &maxSupportedFeatureLevel, context11.ReleaseAndGetAddressOf());

		device11.As(&device5);
		context11.As(&context4);

		//device11->QueryInterface(IID_ID3D11Device5, (void**)device5.ReleaseAndGetAddressOf());
		//context11->QueryInterface(IID_ID3D11DeviceContext4, (void**)context4.ReleaseAndGetAddressOf());

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
		sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 16;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		ComPtr<IDXGISwapChain1> sc;
		dxgiFactory->CreateSwapChainForHwnd(Renderer::getDevice(), hWnd, &sd, nullptr, nullptr, sc.ReleaseAndGetAddressOf());
		sc.As(&swapChain);
	}

	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

	swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)backBuffer.ReleaseAndGetAddressOf());

	if (msaaLevel > 1)
	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = width;
		tDesc.Height = height;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		tDesc.SampleDesc.Count = msaaLevel;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

		Renderer::getDevice()->CreateTexture2D(&tDesc, nullptr, msaaTexture.ReleaseAndGetAddressOf());
	}

	if (enableDebug)
	{
		Renderer::getDevice()->QueryInterface(IID_ID3D11Debug, (void**)d3dDebug.ReleaseAndGetAddressOf());
	}

	DXGI_ADAPTER_DESC2 adapterDesc;

	dxgiAdapter->GetDesc2(&adapterDesc);

	UINT vendorID = adapterDesc.VendorId;

	std::cout << "[class Renderer] GPU VendorID 0x" << std::hex << vendorID << std::dec << "\n";

	if (vendorID == 0x10DE)
	{
		gpuManufacturer = GPUManufacturer::NVIDIA;
	}
	else if (vendorID == 0x1002 || vendorID == 0x1022)
	{
		gpuManufacturer = GPUManufacturer::AMD;
	}
	else if (vendorID == 0x163C || vendorID == 0x8086 || vendorID == 0x8087)
	{
		gpuManufacturer = GPUManufacturer::INTEL;
	}
}