﻿#include<Aurora/Aurora.h>

Aurora Aurora::instance;

Aurora& Aurora::get()
{
	return instance;
}

int Aurora::iniEngine(const Configuration& config)
{
	usage = config.usage;
	enableDebug = config.enableDebug;
	enableImGui = (config.usage == Configuration::EngineUsage::Normal && config.enableImGui);

	allocateConsole();

	if (enableImGui)
	{
		std::cout << "[class Aurora] enable ImGUI\n";
	}

	Utils::ini();

	Keyboard::ini();

	UINT screenWidth, screenHeight;

	if (config.usage == Configuration::EngineUsage::Wallpaper)
	{
		WallpaperHelper::getSystemResolution(screenWidth, screenHeight);
	}
	else
	{
		screenWidth = config.width;
		screenHeight = config.height;
	}

	iniWindow(config.hInstance, config.title, screenWidth, screenHeight);

	if (config.usage == Configuration::EngineUsage::AnimationRender)
	{
		Renderer::instance = new Renderer(hwnd, screenWidth, screenHeight, enableDebug, config.msaaLevel, D3D11_CREATE_DEVICE_VIDEO_SUPPORT);
	}
	else
	{
		Renderer::instance = new Renderer(hwnd, screenWidth, screenHeight, enableDebug, config.msaaLevel);
	}

	States::instance = new States();

	Graphics::instance = new Graphics(screenWidth, screenHeight, config.msaaLevel);

	Camera::instance = new Camera();

	if (usage == Configuration::EngineUsage::AnimationRender)
	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = screenWidth;
		tDesc.Height = screenHeight;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

		Renderer::device->CreateTexture2D(&tDesc, nullptr, encodeTexture.ReleaseAndGetAddressOf());

		RenderAPI::instance = new RenderAPI(Graphics::instance->msaaLevel, encodeTexture.Get());
	}
	else
	{
		RenderAPI::instance = new RenderAPI(Graphics::instance->msaaLevel, Renderer::instance->backBuffer.Get());
	}

	TextureCube::iniShader();

	ResManager::instance = new ResManager();

	if (enableDebug)
	{
		Renderer::device->QueryInterface(IID_ID3D11Debug, (void**)Renderer::instance->d3dDebug.ReleaseAndGetAddressOf());
	}

	//初始化一些默认状态，比如Viewport、BlendState等等 
	switch (config.cameraType)
	{
	default:
	case Configuration::CameraType::Orthogonal:
		std::cout << "[class Aurora] orthogonal camera\n";
		Camera::setProj(DirectX::XMMatrixOrthographicOffCenterLH(0.f, (float)Graphics::getWidth(), 0, (float)Graphics::getHeight(), -1.f, 1.f));
		Camera::setView(DirectX::XMMatrixIdentity());
		break;
	case Configuration::CameraType::Perspective:
		std::cout << "[class Aurora] perspective camera\n";
		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);
		break;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(Renderer::device, Renderer::context);

	RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

	RenderAPI::get()->OMSetBlendState(States::defBlendState);

	RenderAPI::get()->RSSetState(States::rasterCullBack);

	RenderAPI::get()->OMSetDepthStencilState(States::defDepthStencilState, 0);

	RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);

	//pixel compute shader占用第一个槽位来获取跟时间相关的变量
	RenderAPI::get()->PSSetConstantBuffer({ Graphics::instance->deltaTimeBuffer }, 0);
	RenderAPI::get()->CSSetConstantBuffer({ Graphics::instance->deltaTimeBuffer }, 0);

	//vertex geometry hull domain shader占用前两个槽位来获取矩阵信息或者摄像头的信息
	RenderAPI::get()->VSSetConstantBuffer({ Camera::instance->projBuffer,Camera::instance->viewBuffer }, 0);
	RenderAPI::get()->HSSetConstantBuffer({ Camera::instance->projBuffer,Camera::instance->viewBuffer }, 0);
	RenderAPI::get()->DSSetConstantBuffer({ Camera::instance->projBuffer,Camera::instance->viewBuffer }, 0);
	RenderAPI::get()->GSSetConstantBuffer({ Camera::instance->projBuffer,Camera::instance->viewBuffer }, 0);

	return 0;
}

void Aurora::iniGame(Game* const game)
{
	this->game = game;

	switch (usage)
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

	delete Graphics::instance;

	delete States::instance;

	delete Camera::instance;

	delete ResManager::instance;

	delete RenderAPI::instance;

	TextureCube::releaseShader();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (enableDebug)
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

	if (usage == Configuration::EngineUsage::Wallpaper)
	{
		WallpaperHelper::unregisterHOOK();
	}
}

LRESULT Aurora::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}

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

HRESULT Aurora::iniWindow(const HINSTANCE& hInstance, const std::wstring& title, const UINT& width, const UINT& height)
{
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wcex.lpszClassName = L"MyWindowClass";
	wcex.hInstance = hInstance;

	LRESULT(*wndProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) = nullptr;

	DWORD wndStyle = normalWndStyle;

	switch (usage)
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

	RECT rect = { 0,0,(LONG)width,(LONG)height };

	AdjustWindowRect(&rect, wndStyle, false);

	if (usage == Configuration::EngineUsage::Wallpaper)
	{
		hwnd = CreateWindow(L"MyWindowClass", title.c_str(), wndStyle, 0, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);
	}
	else
	{
		hwnd = CreateWindow(L"MyWindowClass", title.c_str(), wndStyle, 100, 100, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);
	}

	if (!hwnd)
	{
		return S_FALSE;
	}

	ShowWindow(hwnd, SW_SHOW);

	if (usage == Configuration::EngineUsage::Wallpaper)
	{
		WallpaperHelper::registerHOOK();
		HWND bg = WallpaperHelper::getWallpaperWindow();
		SetParent(hwnd, bg);
		MoveWindow(hwnd, 0, 0, width, height, 0);
	}
	else if (usage == Configuration::EngineUsage::AnimationRender)
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

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Debug Window");
		ImGui::Text("FrameTime %.8f", ImGui::GetIO().DeltaTime * 1000.f);
		ImGui::Text("FrameRate %.1f", ImGui::GetIO().Framerate);

		const std::chrono::steady_clock::time_point timeStart = timer.now();
		game->update(Graphics::getDeltaTime());
		game->imGUICall();
		game->render();

		ImGui::End();
		ImGui::Render();

		if (enableImGui)
		{
			RenderAPI::get()->OMSetDefRTV(nullptr);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		if (Graphics::instance->msaaLevel != 1)
		{
			Renderer::context->ResolveSubresource(Renderer::instance->backBuffer.Get(), 0, Renderer::instance->msaaTexture.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
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
	bool initializeStatus;

	NvidiaEncoder nvidiaEncoder(Graphics::getWidth(), Graphics::getHeight(), Graphics::instance->recordConfig.frameToEncode, Graphics::instance->recordConfig.frameRate, encodeTexture.Get(), initializeStatus);

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
		if (Graphics::instance->msaaLevel != 1)
		{
			Renderer::context->ResolveSubresource(encodeTexture.Get(), 0, Renderer::instance->msaaTexture.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
		}
		Graphics::instance->deltaTime.sTime += Graphics::instance->deltaTime.deltaTime;
		Graphics::instance->updateDeltaTimeBuffer();
	} while (nvidiaEncoder.encode());

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
	hwnd(0), game(nullptr), enableDebug(false), enableImGui(false), usage(Configuration::EngineUsage::Normal)
{

}
