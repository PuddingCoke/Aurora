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

	int screenWidth, screenHeight;

	if (config.usage == Configuration::EngineUsage::Wallpaper)
	{
		WallpaperHelper::getSystemResolution(screenWidth, screenHeight);
	}
	else
	{
		screenWidth = config.width;
		screenHeight = config.height;
	}

	iniWindow();
	
	Renderer::instance = new Renderer(hwnd, screenWidth, screenHeight, config.enableDebug, config.msaaLevel);

	States::instance = new States();

	Graphics::instance = new Graphics(screenWidth, screenHeight, config.msaaLevel);

	Camera::instance = new Camera();

	Shader::ini();

	TextureCube::iniShader();

	ShadowMap::ini();

	if (config.enableDebug)
	{
		Renderer::device->QueryInterface(IID_ID3D11Debug, (void**)Renderer::instance->d3dDebug.ReleaseAndGetAddressOf());
	}

	//初始化一些默认状态，比如Viewport、BlendState等等 
	switch (config.cameraType)
	{
	default:
	case Configuration::CameraType::Orthogonal:
		std::cout << "[class Aurora] orthogonal camera\n";
		Camera::setProj(DirectX::XMMatrixOrthographicOffCenterLH(0.f, (float)config.width, 0, (float)config.height, 0.f, 1.f));
		Camera::setView(DirectX::XMMatrixIdentity());
		break;
	case Configuration::CameraType::Perspective:
		std::cout << "[class Aurora] perspective camera\n";
		Camera::setProj(Math::pi / 4.f, (float)config.width / (float)config.height, 0.1f, 1000.f);
		break;
	}

	Renderer::setViewport(Graphics::getWidth(), Graphics::getHeight());

	Renderer::setBlendState(States::get()->defBlendState.Get());

	Renderer::context->RSSetState(States::get()->rasterCullBack.Get());

	Renderer::context->OMSetDepthStencilState(States::get()->defDepthStencilState.Get(), 0);

	Renderer::clearDefRTV(DirectX::Colors::Black);

	//pixel compute shader占用第一个槽位来获取跟时间相关的变量
	Renderer::context->PSSetConstantBuffers(0, 1, Graphics::instance->deltaTimeBuffer.GetAddressOf());
	Renderer::context->CSSetConstantBuffers(0, 1, Graphics::instance->deltaTimeBuffer.GetAddressOf());

	//vertex geometry hull domain shader占用前两个槽位来获取矩阵信息或者摄像头的信息
	{
		ID3D11Buffer* const buffers[2] = { Camera::getProjBuffer(),Camera::getViewBuffer() };

		Renderer::context->VSSetConstantBuffers(0, 2, buffers);
		Renderer::context->HSSetConstantBuffers(0, 2, buffers);
		Renderer::context->DSSetConstantBuffers(0, 2, buffers);
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

	//解绑并释放所有资源
	Renderer::context->ClearState();

	delete game;

	delete States::instance;

	delete Graphics::instance;

	delete Camera::instance;

	Shader::release();

	TextureCube::releaseShader();

	ShadowMap::release();

	if (config->enableDebug)
	{
		std::cout << "[class Aurora] debug mode press any key to exit\n";
		Renderer::instance->d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		delete Renderer::instance;
		std::cin.get();
	}
	else
	{
		delete Renderer::instance;
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
		const float curY = (float)Graphics::getHeight() - (float)GET_Y_LPARAM(lParam);

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

	RECT rect = { 0,0,config->width,config->height };

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
		MoveWindow(window, 0, 0, config->width, config->height, 0);
	}
	else if (config->usage == Configuration::EngineUsage::AnimationRender)
	{
		ShowWindow(hwnd, SW_HIDE);
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
		game->update(Graphics::getDeltaTime());
		game->render();
		if (config->msaaLevel != 1)
		{
			Renderer::context->ResolveSubresource(Renderer::instance->backBuffer.Get(), 0, Renderer::instance->msaaTexture.Get(), 0, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		}
		Renderer::instance->swapChain->Present(1, 0);
		const std::chrono::steady_clock::time_point timeEnd = timer.now();
		Graphics::instance->deltaTime.deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() / 1000.f;
		Graphics::instance->deltaTime.sTime += Graphics::instance->deltaTime.deltaTime;
		Graphics::instance->updateDeltaTimeBuffer();
	}
}

void Aurora::runEncode()
{
	ComPtr<ID3D11Texture2D> encodeTexture;
	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = config->width;
		tDesc.Height = config->height;
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

	NvidiaEncoder nvidiaEncoder(Graphics::getWidth(), Graphics::getHeight(), Graphics::instance->recordConfig.frameToEncode, Graphics::instance->recordConfig.frameRate, initializeStatus);

	if (initializeStatus)
	{
		std::cout << "[class Aurora] initialize encoder successfully\n";
	}
	else
	{
		std::cout << "[class Aurora] failed to initialize encoder\n";
		return;
	}

	Graphics::instance->deltaTime.deltaTime = 1.f / 60.f;
	do
	{
		game->update(Graphics::getDeltaTime());
		game->render();
		if (config->msaaLevel != 1)
		{
			Renderer::context->ResolveSubresource(encodeTexture.Get(), 0, Renderer::instance->msaaTexture.Get(), 0, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		}
		else
		{
			Renderer::context->CopyResource(encodeTexture.Get(), Renderer::instance->backBuffer.Get());
		}
		Graphics::instance->deltaTime.sTime += Graphics::instance->deltaTime.deltaTime;
		Graphics::instance->updateDeltaTimeBuffer();
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
