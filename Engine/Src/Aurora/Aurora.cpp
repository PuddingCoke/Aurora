#include<Aurora/Aurora.h>

Aurora Aurora::instance;

Aurora& Aurora::get()
{
	return instance;
}

#pragma managed(push, off)
void InitConsole()
{
	if (AllocConsole())
	{
		FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;

		freopen_s(&fpstdin, "CONIN$", "r", stdin);
		freopen_s(&fpstdout, "CONOUT$", "w", stdout);
		freopen_s(&fpstderr, "CONOUT$", "w", stderr);
	}
}
#pragma managed(pop)

int Aurora::iniEngine(const Configuration& config)
{
	InitConsole();

	Keyboard::ini();

	Random::ini();

	this->config = &config;

	Graphics::width = config.width;
	Graphics::height = config.height;
	Graphics::aspectRatio = (float)Graphics::width / (float)Graphics::height;
	Graphics::msaaLevel = config.msaaLevel;

	std::cout << "[class Aurora] multisample level " << config.msaaLevel << "\n";

	if (SUCCEEDED(iniWindow()))
	{
		std::cout << "[class Aurora] Create window successfully!\n";
	}
	else
	{
		std::cout << "[class Aurora] Create window failed!\n";
		return 1;
	}

	if (SUCCEEDED(iniDevice()))
	{
		std::cout << "[class Aurora] Create device successfully!\n";
	}
	else
	{
		std::cout << "[class Aurora] Create device failed!\n";
	}

	if (SUCCEEDED(iniGameConstant()))
	{
		std::cout << "[class Aurora] Create game constants successfully!\n";
	}
	else
	{
		std::cout << "[class Aurora] Create game constants failed!\n";
	}

	if (SUCCEEDED(StateCommon::ini()))
	{
		std::cout << "[class Aurora] StateCommon initialize successfully\n";
	}
	else
	{
		std::cout << "[class Aurora] StateCommon initialize failed\n";
	}

	Shader::ini();

	return 0;
}

void Aurora::iniGame(Game* const game)
{
	this->game = game;
	switch (config->usage)
	{
	case Configuration::EngineUsage::Normal:
		runGame();
		break;
	default:

		break;
	}

	delete game;

	Shader::release();

	Graphics::context->ClearState();
}

LRESULT Aurora::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_MOUSEMOVE:
	{
		const float curX = (float)GET_X_LPARAM(lParam);
		const float curY = (float)config->height - (float)GET_Y_LPARAM(lParam);

		Mouse::dx = curX - Mouse::x;
		Mouse::dy = curY - Mouse::y;
		Mouse::x = curX;
		Mouse::y = curY;

		Mouse::moveEvent();

	}
	break;

	case WM_LBUTTONDOWN:
		Mouse::leftDown = true;
		Mouse::leftDownEvent();
		break;

	case WM_RBUTTONDOWN:
		Mouse::rightDown = true;
		Mouse::rightDownEvent();
		break;

	case WM_LBUTTONUP:
		Mouse::leftDown = false;
		Mouse::leftUpEvent();
		break;

	case WM_RBUTTONUP:
		Mouse::rightDown = false;
		Mouse::rightUpEvent();
		break;

	case WM_KEYDOWN:
		if ((HIWORD(lParam) & KF_REPEAT) == 0)
		{
			Keyboard::keyDownMap[(Keyboard::Key)wParam] = true;
			Keyboard::keyDownEvents[(Keyboard::Key)wParam]();
		}
		break;

	case WM_KEYUP:
		Keyboard::keyDownMap[(Keyboard::Key)wParam] = false;
		Keyboard::keyUpEvents[(Keyboard::Key)wParam]();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

HRESULT Aurora::iniWindow()
{
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;

	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);

	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);

	wcex.lpszClassName = L"MyWindowClass";

	wcex.hInstance = config->hInstance;

	auto  wndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		return Aurora::get().WindowProc(hwnd, msg, wParam, lParam);
	};

	wcex.lpfnWndProc = wndProc;

	RegisterClassEx(&wcex);

	RECT rect = { 0,0,config->width,config->height };

	AdjustWindowRect(&rect, WS_CAPTION | WS_SYSMENU, false);

	std::cout << "[class Aurora] Window size " << rect.right - rect.left << " " << rect.bottom - rect.top << "\n";

	hwnd = CreateWindow(L"MyWindowClass", config->title.c_str(), WS_CAPTION | WS_SYSMENU, 100, 100, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, config->hInstance, nullptr);

	if (!hwnd)
	{
		return S_FALSE;
	}

	ShowWindow(hwnd, SW_SHOW);

	return S_OK;
}

HRESULT Aurora::iniDevice()
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

		D3D11_CREATE_DEVICE_FLAG deviceFlag;

		if (config->enableDebug)
		{
			std::cout << "[class Aurora] Enable debug!\n";
			deviceFlag = D3D11_CREATE_DEVICE_DEBUG;
		}
		else
		{
			deviceFlag = (D3D11_CREATE_DEVICE_FLAG)0;
		}

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlag, featureLevels, 2u,
			D3D11_SDK_VERSION, device11.GetAddressOf(), &maxSupportedFeatureLevel, context11.GetAddressOf());

		device11.As(&Graphics::device);
		context11.As(&Graphics::context);

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
		sd.Width = config->width;
		sd.Height = config->height;
		sd.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 3;
		sd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;

		ComPtr<IDXGISwapChain1> sc;
		dxgiFactory->CreateSwapChainForHwnd(Graphics::device.Get(), hwnd, &sd, nullptr, nullptr, sc.GetAddressOf());
		sc.As(&swapChain);
	}

	dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);

	Graphics::vp.MinDepth = 0.0f;
	Graphics::vp.MaxDepth = 1.0f;
	Graphics::vp.TopLeftX = 0;
	Graphics::vp.TopLeftY = 0;

	Graphics::setViewport((float)config->width, (float)config->height);

	{

		D3D11_BLEND_DESC blendStateDesc = {};

		blendStateDesc.IndependentBlendEnable = false;

		blendStateDesc.RenderTarget[0].BlendEnable = true;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		Graphics::device->CreateBlendState(&blendStateDesc, StateCommon::defBlendState.ReleaseAndGetAddressOf());

		Graphics::setBlendState(StateCommon::defBlendState.Get());

	}

	{
		ComPtr<ID3D11RasterizerState> rasterizerState;

		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		Graphics::device->CreateRasterizerState(&rasterizerDesc, rasterizerState.ReleaseAndGetAddressOf());

		Graphics::context->RSSetState(rasterizerState.Get());

	}

	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = config->width;
		tDesc.Height = config->height;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		tDesc.SampleDesc.Count = Graphics::msaaLevel;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		tDesc.CPUAccessFlags = 0;
		tDesc.MiscFlags = 0;

		Graphics::device->CreateTexture2D(&tDesc, nullptr, msaaTexture.ReleaseAndGetAddressOf());

		D3D11_RENDER_TARGET_VIEW_DESC msaaViewDesc = {};
		msaaViewDesc.Format = tDesc.Format;
		msaaViewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DMS;
		msaaViewDesc.Texture2D.MipSlice = 0;

		Graphics::device->CreateRenderTargetView(msaaTexture.Get(), &msaaViewDesc, Graphics::defaultTargetView.ReleaseAndGetAddressOf());

		Graphics::clearDefRTV(DirectX::Colors::Black);

	}

	return S_OK;
}

HRESULT Aurora::iniGameConstant()
{
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = sizeof(DirectX::XMMATRIX);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Graphics::device->CreateBuffer(&cbd, nullptr, Graphics::cBufferProj.ReleaseAndGetAddressOf());
		Graphics::device->CreateBuffer(&cbd, nullptr, Graphics::cBufferView.ReleaseAndGetAddressOf());

		switch (config->cameraType)
		{
		default:
		case Configuration::CameraType::Orthogonal:
			Graphics::setProj(DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(0.f, (float)config->width, 0, (float)config->height, 0.f, 1.f)));
			Graphics::setView(DirectX::XMMatrixIdentity());
			break;
		case Configuration::CameraType::Perspective:

			break;
		}

		ID3D11Buffer* buffers[2] = { Graphics::cBufferProj.Get(),Graphics::cBufferView.Get() };

		Graphics::context->VSSetConstantBuffers(0, 2, buffers);
		Graphics::context->GSSetConstantBuffers(0, 2, buffers);

	}

	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = 16u;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Graphics::device->CreateBuffer(&cbd, nullptr, Graphics::cBufferDeltaTimes.ReleaseAndGetAddressOf());
		Graphics::updateGPUDeltaTimes();
		Graphics::context->PSSetConstantBuffers(0, 1, Graphics::cBufferDeltaTimes.GetAddressOf());
	}

	return S_OK;
}

void Aurora::runGame()
{

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timeStart = timer.now();
			game->update(Graphics::deltaTime);
			game->render();
			Graphics::context->ResolveSubresource(backBuffer, 0, msaaTexture.Get(), 0, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
			swapChain->Present(1, 0);
			timeEnd = timer.now();
			Graphics::deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() / 1000.f;
			Graphics::updateGPUDeltaTimes();
		}
	}
}

Aurora::Aurora() :
	hwnd(0), config(nullptr), game(nullptr), backBuffer(nullptr)
{

}
