#include<Aurora/Aurora.h>

Aurora Aurora::instance;

Aurora& Aurora::get()
{
	return instance;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	HWND p = FindWindowEx(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);
	HWND* ret = (HWND*)lParam;

	if (p)
	{
		// Gets the WorkerW Window after the current one.
		*ret = FindWindowEx(nullptr, hwnd, L"WorkerW", nullptr);
	}
	return true;
}

static HWND get_wallpaper_window()
{
	// Fetch the Progman window
	HWND progman = FindWindow(L"ProgMan", nullptr);
	// Send 0x052C to Progman. This message directs Progman to spawn a 
	// WorkerW behind the desktop icons. If it is already there, nothing 
	// happens.
	SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);
	// We enumerate all Windows, until we find one, that has the SHELLDLL_DefView 
	// as a child. 
	// If we found that window, we take its next sibling and assign it to workerw.
	HWND wallpaper_hwnd = nullptr;
	EnumWindows(EnumWindowsProc, (LPARAM)&wallpaper_hwnd);
	// Return the handle you're looking for.
	return wallpaper_hwnd;
}

static void getSysResolution(int& width, int& height)
{
	HMONITOR monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	width = info.rcMonitor.right - info.rcMonitor.left;
	height = info.rcMonitor.bottom - info.rcMonitor.top;
	//以上代码获取的width和height实际上是 windowWidth/dpi windowHeight/dpi所以还得获取屏幕缩放比例

	UINT dpi = GetDpiForWindow(GetDesktopWindow());

	std::cout << "[class Aurora] system dpi " << dpi << "\n";

	width *= dpi / 96.f;
	height *= dpi / 96.f;
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

	if (config.usage == Configuration::EngineUsage::Wallpaper)
	{
		int width;
		int height;
		getSysResolution(width, height);
		Graphics::width = width;
		Graphics::height = height;
	}
	else
	{
		Graphics::width = config.width;
		Graphics::height = config.height;
	}

	Graphics::aspectRatio = (float)Graphics::width / (float)Graphics::height;


	Graphics::msaaLevel = config.msaaLevel;

	std::cout << "[class Aurora] resolution:" << Graphics::width << " " << Graphics::height << "\n";
	std::cout << "[class Aurora] aspectRatio:" << Graphics::aspectRatio << "\n";
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
	default:
	case Configuration::EngineUsage::Wallpaper:
	case Configuration::EngineUsage::Normal:
		runGame();
		break;
	case Configuration::EngineUsage::AnimationRender:
		runEncode();
		break;
	}

	delete game;

	Shader::release();

	Graphics::context->ClearState();

	if (config->enableDebug)
	{
		Graphics::d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}

	if (config->usage == Configuration::EngineUsage::Wallpaper)
	{
		UnhookWindowsHookEx(mouseHook);
	}
}

LRESULT __stdcall Aurora::WallpaperMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

		switch (wParam)
		{
		case WM_MOUSEMOVE:
		{
			const float curX = (float)pMouseStruct->pt.x;
			const float curY = (float)Graphics::height - (float)pMouseStruct->pt.y;

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
		}

	}
	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
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
		const float curY = (float)Graphics::height - (float)GET_Y_LPARAM(lParam);

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

LRESULT Aurora::WallpaperProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

	LRESULT(*wndProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) = nullptr;

	DWORD wndStyle = normalWndStyle;

	switch (config->usage)
	{
	default:
	case Configuration::EngineUsage::Normal:
		std::cout << "[class Aurora] usage normal\n";
		wndStyle = normalWndStyle;
		wndProc= [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
		{
			return Aurora::get().WindowProc(hwnd, msg, wParam, lParam);
		};
		break;

	case Configuration::EngineUsage::Wallpaper:
		std::cout << "[class Aurora] usage wallpaper\n";
		wndStyle = wallpaperWndStyle;
		wndProc= [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
		{
			return Aurora::get().WallpaperProc(hwnd, msg, wParam, lParam);
		};
		break;

	case Configuration::EngineUsage::AnimationRender:
		std::cout << "[class Aurora] usage animation render\n";
		wndStyle = normalWndStyle;
		wndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
		{
			return Aurora::get().WallpaperProc(hwnd, msg, wParam, lParam);
		};
		break;
	}

	wcex.lpfnWndProc = wndProc;

	RegisterClassEx(&wcex);

	RECT rect = { 0,0,Graphics::width,Graphics::height };

	AdjustWindowRect(&rect, wndStyle, false);

	if (config->usage == Configuration::EngineUsage::Wallpaper)
	{
		hwnd = CreateWindow(L"MyWindowClass", config->title.c_str(), wndStyle, 0, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, config->hInstance, nullptr);
	}
	else
	{
		hwnd = CreateWindow(L"MyWindowClass", config->title.c_str(), wndStyle, 100, 100, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, config->hInstance, nullptr);
	}

	if (!hwnd)
	{
		return S_FALSE;
	}
	
	ShowWindow(hwnd, SW_SHOW);

	if (config->usage == Configuration::EngineUsage::Wallpaper)
	{
		auto mouseProc = [](int nCode, WPARAM wParam, LPARAM lParam)
		{
			return Aurora::get().WallpaperMouseProc(nCode, wParam, lParam);
		};

		mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, NULL, 0);

		HWND window = FindWindowW(nullptr, config->title.c_str());
		HWND bg = get_wallpaper_window();
		SetParent(window, bg);
		MoveWindow(window, 0, 0, Graphics::width, Graphics::height, 0);
	}
	else if (config->usage == Configuration::EngineUsage::AnimationRender)
	{
		ShowWindow(hwnd, SW_HIDE);
	}

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
			std::cout << "[class Aurora] Disable debug!\n";
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
		sd.Width = Graphics::width;
		sd.Height = Graphics::height;
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

	if (config->enableDebug)
	{
		Graphics::device->QueryInterface(IID_ID3D11Debug, (void**)Graphics::d3dDebug.ReleaseAndGetAddressOf());
	}

	swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&Graphics::backBuffer);

	Graphics::vp.MinDepth = 0.0f;
	Graphics::vp.MaxDepth = 1.0f;
	Graphics::vp.TopLeftX = 0;
	Graphics::vp.TopLeftY = 0;

	Graphics::setViewport((float)Graphics::width, (float)Graphics::height);

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
		tDesc.Width = Graphics::width;
		tDesc.Height = Graphics::height;
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

	if (config->usage == Configuration::EngineUsage::AnimationRender)
	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = Graphics::width;
		tDesc.Height = Graphics::height;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		tDesc.CPUAccessFlags = 0;
		tDesc.MiscFlags = 0;

		Graphics::device->CreateTexture2D(&tDesc, nullptr, Graphics::encodeTexture.ReleaseAndGetAddressOf());

		std::cout << "[class Aurora] initialize encode texture complete\n";
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
			std::cout << "[class Aurora] Orthogonal camera\n";
			Graphics::setProj(DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(0.f, (float)Graphics::width, 0, (float)Graphics::height, 0.f, 1.f)));
			Graphics::setView(DirectX::XMMatrixIdentity());
			break;
		case Configuration::CameraType::Perspective:
			std::cout << "[class Aurora] Perspective camera\n";
			Graphics::setProj(DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(Math::pi / 4.f, Graphics::aspectRatio, 0.1f, 1000.f)));
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
		Graphics::context->CSSetConstantBuffers(0, 1, Graphics::cBufferDeltaTimes.GetAddressOf());
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
		timeStart = timer.now();
		game->update(Graphics::deltaTime);
		game->render();
		Graphics::context->ResolveSubresource(Graphics::backBuffer, 0, msaaTexture.Get(), 0, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		swapChain->Present(1, 0);
		timeEnd = timer.now();
		Graphics::deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() / 1000.f;
		Graphics::sTime += Graphics::deltaTime;
		Graphics::updateGPUDeltaTimes();
	}
}

void Aurora::runEncode()
{
	bool initializeStatus;

	NvidiaEncoder nvidiaEncoder(3600, 60, initializeStatus);

	if (initializeStatus)
	{
		std::cout << "[class Aurora] Initialize encoder successfully\n";
	}
	else
	{
		std::cout << "[class Aurora] Failed to initialize encoder\n";
		return;
	}

	Graphics::deltaTime = 1.f / 60.f;
	do
	{
		game->update(Graphics::deltaTime);
		game->render();
		Graphics::context->ResolveSubresource(Graphics::encodeTexture.Get(), 0, msaaTexture.Get(), 0, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		Graphics::sTime += Graphics::deltaTime;
		Graphics::updateGPUDeltaTimes();
	} while (nvidiaEncoder.encode());

	std::cin.get();
}

Aurora::Aurora() :
	hwnd(0), config(nullptr), game(nullptr),mouseHook(nullptr)
{

}
