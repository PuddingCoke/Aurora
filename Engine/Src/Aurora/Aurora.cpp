#include<Aurora/Aurora.h>

Aurora Aurora::instance;

Aurora& Aurora::get()
{
	return instance;
}

int Aurora::iniEngine(const Configuration& config)
{
	allocateConsole();

	Keyboard::ini();

	this->config = &config;

	if (config.usage == Configuration::EngineUsage::Wallpaper)
	{
		int width;
		int height;
		WallpaperHelper::getSystemResolution(width, height);
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
	std::cout << "[class Aurora] multisample level:" << config.msaaLevel << "\n";

	if (SUCCEEDED(iniWindow()))
	{
		std::cout << "[class Aurora] create window successfully!\n";
	}
	else
	{
		std::cout << "[class Aurora] create window failed!\n";
		return 1;
	}

	if (SUCCEEDED(iniDevice()))
	{
		std::cout << "[class Aurora] create device successfully!\n";
	}
	else
	{
		std::cout << "[class Aurora] create device failed!\n";
	}

	if (SUCCEEDED(iniCamera()))
	{
		std::cout << "[class Aurora] initialize camera successfully!\n";
	}
	else
	{
		std::cout << "[class Aurora] initialize camera failed!\n";
	}

	if (SUCCEEDED(States::ini()))
	{
		std::cout << "[class Aurora] States initialize successfully\n";
	}
	else
	{
		std::cout << "[class Aurora] States initialize failed\n";
	}

	Shader::ini();

	TextureCube::iniShader();

	Graphics::ini();

	ShadowMap::ini();

	//初始化一些默认状态，比如Viewport、BlendState等等 
	Renderer::setViewport(Graphics::width, Graphics::height);

	Renderer::setBlendState(States::defBlendState.Get());

	Renderer::context->RSSetState(States::rasterCullBack.Get());

	Renderer::context->OMSetDepthStencilState(States::defDepthStencilState.Get(), 0);

	Renderer::clearDefRTV(DirectX::Colors::Black);

	//pixel compute shader占用第一个槽位来获取跟时间相关的变量
	Renderer::context->PSSetConstantBuffers(0, 1, Graphics::deltaTimeBuffer.GetAddressOf());
	Renderer::context->CSSetConstantBuffers(0, 1, Graphics::deltaTimeBuffer.GetAddressOf());

	//vertex geometry shader占用前两个槽位来获取矩阵信息或者摄像头的信息
	{
		ID3D11Buffer* const buffers[2] = { Camera::projBuffer.Get(),Camera::viewBuffer.Get() };

		Renderer::context->VSSetConstantBuffers(0, 2, buffers);
		Renderer::context->GSSetConstantBuffers(0, 2, buffers);
	}

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

	Renderer::context->ClearState();

	delete game;

	Shader::release();

	TextureCube::releaseShader();

	ShadowMap::release();

	Renderer::backBuffer->Release();

	if (config->enableDebug)
	{
		Graphics::d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		std::cout << "[class Aurora] debug mode press any key to exit\n";
		std::cin.get();
	}

	if (config->usage == Configuration::EngineUsage::Wallpaper)
	{
		WallpaperHelper::unregisterHOOK();
	}
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

	case WM_MOUSEWHEEL:
		Mouse::wheelDelta = (float)GET_WHEEL_DELTA_WPARAM(wParam) / 120.f;
		Mouse::scrollEvent();
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
		wndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
		{
			return Aurora::get().WindowProc(hwnd, msg, wParam, lParam);
		};
		break;

	case Configuration::EngineUsage::Wallpaper:
		std::cout << "[class Aurora] usage wallpaper\n";
		wndStyle = wallpaperWndStyle;
		wndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
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
		WallpaperHelper::registerHOOK();
		HWND window = FindWindowW(nullptr, config->title.c_str());
		HWND bg = WallpaperHelper::getWallpaperWindow();
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
			std::cout << "[class Aurora] enable debug!\n";
			deviceFlag = D3D11_CREATE_DEVICE_DEBUG;
		}
		else
		{
			std::cout << "[class Aurora] disable debug!\n";
			deviceFlag = (D3D11_CREATE_DEVICE_FLAG)0;
		}

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlag, featureLevels, 2u,
			D3D11_SDK_VERSION, device11.GetAddressOf(), &maxSupportedFeatureLevel, context11.GetAddressOf());

		device11.As(&Renderer::device);
		context11.As(&Renderer::context);

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
		dxgiFactory->CreateSwapChainForHwnd(Renderer::device.Get(), hwnd, &sd, nullptr, nullptr, sc.GetAddressOf());
		sc.As(&swapChain);
	}

	dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	if (config->enableDebug)
	{
		Renderer::device->QueryInterface(IID_ID3D11Debug, (void**)Graphics::d3dDebug.ReleaseAndGetAddressOf());
	}

	{
		swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&Renderer::backBuffer);

		if (Graphics::msaaLevel == 1)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			Renderer::device->CreateRenderTargetView(Renderer::backBuffer, &rtvDesc, Renderer::defaultTargetView.ReleaseAndGetAddressOf());
		}
		else
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

			Renderer::device->CreateTexture2D(&tDesc, nullptr, msaaTexture.ReleaseAndGetAddressOf());

			D3D11_RENDER_TARGET_VIEW_DESC msaaViewDesc = {};
			msaaViewDesc.Format = tDesc.Format;
			msaaViewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DMS;
			msaaViewDesc.Texture2D.MipSlice = 0;

			Renderer::device->CreateRenderTargetView(msaaTexture.Get(), &msaaViewDesc, Renderer::defaultTargetView.ReleaseAndGetAddressOf());
		}
	}

	return S_OK;
}

HRESULT Aurora::iniCamera()
{
	Camera::initialize();

	switch (config->cameraType)
	{
	default:
	case Configuration::CameraType::Orthogonal:
		std::cout << "[class Aurora] orthogonal camera\n";
		Camera::setProj(DirectX::XMMatrixOrthographicOffCenterLH(0.f, (float)Graphics::width, 0, (float)Graphics::height, 0.f, 1.f));
		Camera::setView(DirectX::XMMatrixIdentity());
		break;
	case Configuration::CameraType::Perspective:
		std::cout << "[class Aurora] perspective camera\n";
		Camera::setProj(Math::pi / 4.f, Graphics::aspectRatio, 0.1f, 1000.f);
		break;
	}

	return S_OK;
}

void Aurora::runGame()
{
	std::chrono::steady_clock timer;

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		const std::chrono::steady_clock::time_point timeStart = timer.now();
		game->update(Graphics::deltaTime.deltaTime);
		game->render();
		if (config->msaaLevel != 1)
		{
			Renderer::context->ResolveSubresource(Renderer::backBuffer, 0, msaaTexture.Get(), 0, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		}
		swapChain->Present(1, 0);
		const std::chrono::steady_clock::time_point timeEnd = timer.now();
		Graphics::deltaTime.deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() / 1000.f;
		Graphics::deltaTime.sTime += Graphics::deltaTime.deltaTime;
		Graphics::updateDeltaTimeBuffer();
	}
}

void Aurora::runEncode()
{
	ComPtr<ID3D11Texture2D> encodeTexture;
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

		Renderer::device->CreateTexture2D(&tDesc, nullptr, encodeTexture.ReleaseAndGetAddressOf());

		std::cout << "[class Aurora] initialize encode texture complete\n";
	}

	bool initializeStatus;

	NvidiaEncoder nvidiaEncoder(Graphics::getWidth(), Graphics::getHeight(), Graphics::recordConfig.frameToEncode, Graphics::recordConfig.frameRate, initializeStatus);

	if (initializeStatus)
	{
		std::cout << "[class Aurora] initialize encoder successfully\n";
	}
	else
	{
		std::cout << "[class Aurora] failed to initialize encoder\n";
		return;
	}

	Graphics::deltaTime.deltaTime = 1.f / 60.f;
	do
	{
		game->update(Graphics::deltaTime.deltaTime);
		game->render();
		if (config->msaaLevel != 1)
		{
			Renderer::context->ResolveSubresource(encodeTexture.Get(), 0, msaaTexture.Get(), 0, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		}
		else
		{
			Renderer::context->CopyResource(encodeTexture.Get(), Renderer::backBuffer);
		}
		Graphics::deltaTime.sTime += Graphics::deltaTime.deltaTime;
		Graphics::updateDeltaTimeBuffer();
	} while (nvidiaEncoder.encode(encodeTexture.Get()));

	std::cout << "[class Aurora] encode complete!\n";

	std::cin.get();
}

void Aurora::allocateConsole()
{
	if (AllocConsole())
	{
		FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;

		freopen_s(&fpstdin, "CONIN$", "r", stdin);
		freopen_s(&fpstdout, "CONOUT$", "w", stdout);
		freopen_s(&fpstderr, "CONOUT$", "w", stderr);
	}
}

Aurora::Aurora() :
	hwnd(0), config(nullptr), game(nullptr)
{

}
