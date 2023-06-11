#include<Aurora/Core/Renderer.h>

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

Renderer* Renderer::get()
{
	return instance;
}

void Renderer::createBuffer(const D3D11_BUFFER_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Buffer** address) const
{
	CHECKERROR(device5->CreateBuffer(desc, initialData, address));
}

void Renderer::createTexture2D(const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture2D** address) const
{
	CHECKERROR(device5->CreateTexture2D(desc, initialData, address));
}

void Renderer::createTexture3D(const D3D11_TEXTURE3D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture3D** address) const
{
	CHECKERROR(device5->CreateTexture3D(desc, initialData, address));
}

void Renderer::createDepthStencilView(ID3D11Resource* resource, const D3D11_DEPTH_STENCIL_VIEW_DESC* desc, ID3D11DepthStencilView** address) const
{
	CHECKERROR(device5->CreateDepthStencilView(resource, desc, address));
}

void Renderer::createShaderResourceView(ID3D11Resource* resource, const D3D11_SHADER_RESOURCE_VIEW_DESC* desc, ID3D11ShaderResourceView** address) const
{
	CHECKERROR(device5->CreateShaderResourceView(resource, desc, address));
}

void Renderer::createRenderTargetView(ID3D11Resource* resource, const D3D11_RENDER_TARGET_VIEW_DESC* desc, ID3D11RenderTargetView** address) const
{
	CHECKERROR(device5->CreateRenderTargetView(resource, desc, address));
}

void Renderer::createUnorderedAccessView(ID3D11Resource* resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC* desc, ID3D11UnorderedAccessView** address) const
{
	CHECKERROR(device5->CreateUnorderedAccessView(resource, desc, address));
}

void Renderer::createVertexShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11VertexShader** address) const
{
	CHECKERROR(device5->CreateVertexShader(byteCode, byteCodeLength, nullptr, address));
}

void Renderer::createHullShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11HullShader** address) const
{
	CHECKERROR(device5->CreateHullShader(byteCode, byteCodeLength, nullptr, address));
}

void Renderer::createDomainShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11DomainShader** address) const
{
	CHECKERROR(device5->CreateDomainShader(byteCode, byteCodeLength, nullptr, address));
}

void Renderer::createGeometryShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11GeometryShader** address) const
{
	CHECKERROR(device5->CreateGeometryShader(byteCode, byteCodeLength, nullptr, address));
}

void Renderer::createPixelShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11PixelShader** address) const
{
	CHECKERROR(device5->CreatePixelShader(byteCode, byteCodeLength, nullptr, address));
}

void Renderer::createComputeShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11ComputeShader** address) const
{
	CHECKERROR(device5->CreateComputeShader(byteCode, byteCodeLength, nullptr, address));
}

void Renderer::createSamplerState(const D3D11_SAMPLER_DESC* desc, ID3D11SamplerState** address) const
{
	CHECKERROR(device5->CreateSamplerState(desc, address));
}

void Renderer::createInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, UINT arraySize, const void* byteCode, SIZE_T byteCodeLength, ID3D11InputLayout** address) const
{
	CHECKERROR(device5->CreateInputLayout(desc, arraySize, byteCode, byteCodeLength, address));
}

void Renderer::createDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* desc, ID3D11DepthStencilState** address) const
{
	CHECKERROR(device5->CreateDepthStencilState(desc, address));
}

void Renderer::createBlendState(const D3D11_BLEND_DESC* desc, ID3D11BlendState** address) const
{
	CHECKERROR(device5->CreateBlendState(desc, address));
}

void Renderer::createRasterizerState(const D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** address) const
{
	CHECKERROR(device5->CreateRasterizerState(desc, address));
}

void Renderer::createRasterizerState1(const D3D11_RASTERIZER_DESC1* desc, ID3D11RasterizerState1** address) const
{
	CHECKERROR(device5->CreateRasterizerState1(desc, address));
}

void Renderer::createRasterizerState2(const D3D11_RASTERIZER_DESC2* desc, ID3D11RasterizerState2** address) const
{
	CHECKERROR(device5->CreateRasterizerState2(desc, address));
}

Renderer::Renderer(HWND hWnd, const UINT& width, const UINT& height, const UINT& msaaLevel) :
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

#ifdef _DEBUG
		std::cout << "[class Renderer] enable debug\n";
		UINT deviceFlag = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;
#else
		std::cout << "[class Renderer] disable debug\n";
		UINT deviceFlag = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
#endif // _DEBUG

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlag, featureLevels, ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION, device11.ReleaseAndGetAddressOf(), &maxSupportedFeatureLevel, context11.ReleaseAndGetAddressOf());

		device11.As(&device5);
		context11.As(&context4);

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

		Renderer::get()->createTexture2D(&tDesc, nullptr, msaaTexture.ReleaseAndGetAddressOf());
	}

#ifdef _DEBUG
	Renderer::getDevice()->QueryInterface(IID_ID3D11Debug, (void**)d3dDebug.ReleaseAndGetAddressOf());
#endif // _DEBUG

	DXGI_ADAPTER_DESC2 adapterDesc;

	dxgiAdapter->GetDesc2(&adapterDesc);

	UINT vendorID = adapterDesc.VendorId;

	std::cout << "[class Renderer] GPU VendorID 0x" << std::hex << vendorID << std::dec << " Manufacturer:";

	if (vendorID == 0x10DE)
	{
		std::cout << "NVIDIA\n";
		gpuManufacturer = GPUManufacturer::NVIDIA;
	}
	else if (vendorID == 0x1002 || vendorID == 0x1022)
	{
		std::cout << "AMD\n";
		gpuManufacturer = GPUManufacturer::AMD;
	}
	else if (vendorID == 0x163C || vendorID == 0x8086 || vendorID == 0x8087)
	{
		std::cout << "INTEL\n";
		gpuManufacturer = GPUManufacturer::INTEL;
	}
	else
	{
		std::cout << "UNKNOWN\n";
		gpuManufacturer = GPUManufacturer::UNKNOWN;
	}
}