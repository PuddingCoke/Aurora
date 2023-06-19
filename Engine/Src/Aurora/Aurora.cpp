#include<Aurora/Aurora.h>

Aurora Aurora::instance;

Aurora& Aurora::get()
{
	return instance;
}

int Aurora::iniEngine(const Configuration& config, const int& argc, const char* argv[])
{
	std::wcout.imbue(std::locale(""));

	{
		std::string exeRootPath = argv[0];

		Utils::exeRootPath = Utils::File::backslashToSlash(Utils::File::getParentFolder(exeRootPath));

		std::cout << "[class Aurora] executable path " << Utils::getRootFolder() << "\n";
	}

	usage = config.usage;
	enableImGui = (config.usage == Configuration::EngineUsage::Normal && config.enableImGui);

	if (enableImGui)
	{
		std::cout << "[class Aurora] enable ImGUI\n";
	}

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

	iniWindow(config.title, screenWidth, screenHeight);

	iniRenderer(config.msaaLevel, screenWidth, screenHeight);

	iniStates(config);

	std::cout << "[class Aurora] initialize engine complete\n";

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

	destroy();
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

void Aurora::runGame()
{
	std::chrono::high_resolution_clock timer;

	while (winform->pollEvents())
	{
		if (enableImGui)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Debug Window");
			ImGui::Text("TimeElapsed %.2f", Graphics::getSTime());
			ImGui::Text("FrameTime %.8f", ImGui::GetIO().DeltaTime * 1000.f);
			ImGui::Text("FrameRate %.1f", ImGui::GetIO().Framerate);
		}

		const std::chrono::high_resolution_clock::time_point timeStart = timer.now();

		game->update(Graphics::getDeltaTime());

		if (enableImGui)
		{
			game->imGUICall();
		}

		Graphics::instance->updateDeltaTimeBuffer();

		bindCommonCB();

		game->render();

		if (enableImGui)
		{
			ImGui::End();
			ImGui::Render();

			ImCtx::get()->OMSetDefRTV(nullptr);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		if (Graphics::instance->msaaLevel != 1)
		{
			ImCtx::GetContext()->ResolveSubresource(Renderer::instance->backBuffer.Get(), 0, Renderer::instance->msaaTexture.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
		}

		Renderer::instance->swapChain->Present(1, 0);
		const std::chrono::high_resolution_clock::time_point timeEnd = timer.now();
		Graphics::instance->deltaTime.deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() / 1000.f;
		Graphics::instance->deltaTime.sTime += Graphics::instance->deltaTime.deltaTime;
	}
}

void Aurora::runEncode()
{
	bool initializeStatus;

	NvidiaEncoder nvidiaEncoder(Graphics::getWidth(), Graphics::getHeight(), Graphics::instance->recordConfig.frameToEncode, Graphics::instance->recordConfig.frameRate, encodeTexture->getResource(), initializeStatus);

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

		Graphics::instance->updateDeltaTimeBuffer();

		bindCommonCB();

		game->render();

		if (Graphics::instance->msaaLevel != 1)
		{
			ImCtx::GetContext()->ResolveSubresource(encodeTexture->getResource(), 0, Renderer::instance->msaaTexture.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
		}
		Graphics::instance->deltaTime.sTime += Graphics::instance->deltaTime.deltaTime;
	} while (nvidiaEncoder.encode());

	delete encodeTexture;

	std::cout << "[class Aurora] encode complete!\n";

	std::cin.get();
}

void Aurora::bindCommonCB()
{
	//pixel compute shader占用第一个槽位来获取跟时间相关的变量
	ImCtx::get()->PSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);
	ImCtx::get()->CSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);

	//vertex geometry hull domain shader占用前两个槽位来获取矩阵信息或者摄像头的信息
	ImCtx::get()->VSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
	ImCtx::get()->HSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
	ImCtx::get()->DSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
	ImCtx::get()->GSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
}

void Aurora::destroy()
{
	ImCtx::GetContext()->ClearState();

	delete game;

	delete winform;

	delete Graphics::instance;

	delete States::instance;

	delete Camera::instance;

	delete ImCtx::instance;

	TextureCube::releaseShader();

	if (enableImGui)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

#ifdef _DEBUG
	std::cout << "[class Aurora] debug mode press any key to exit\n";
	Renderer::instance->d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	delete Renderer::instance;
	std::cin.get();
#else
	delete Renderer::instance;
#endif // _DEBUG

	if (usage == Configuration::EngineUsage::Wallpaper)
	{
		WallpaperHelper::unregisterHOOK();
	}
}

Aurora::Aurora() :
	encodeTexture(nullptr), winform(nullptr), game(nullptr), enableImGui(false), usage(Configuration::EngineUsage::Normal)
{

}

void Aurora::iniWindow(const std::wstring& title, const UINT& screenWidth, const UINT& screenHeight)
{
	UINT resolutionX;
	UINT resolutionY;

	WallpaperHelper::getSystemResolution(resolutionX, resolutionY);

	switch (usage)
	{
	default:
	case Configuration::EngineUsage::Normal:
		std::cout << "[class Aurora] usage normal\n";
		winform = new Win32Form(title, (resolutionX - screenWidth) / 2, (resolutionY - screenHeight) / 2, screenWidth, screenHeight, normalWndStyle, Aurora::WindowProc);
		break;

	case Configuration::EngineUsage::Wallpaper:
		std::cout << "[class Aurora] usage wallpaper\n";
		winform = new Win32Form(title, 0, 0, screenWidth, screenHeight, wallpaperWndStyle, Aurora::WallpaperProc);
		break;

	case Configuration::EngineUsage::AnimationRender:
		std::cout << "[class Aurora] usage animation render\n";
		winform = new Win32Form(title, 100, 100, screenWidth, screenHeight, normalWndStyle, Aurora::WallpaperProc);
		break;
	}

	if (usage == Configuration::EngineUsage::Wallpaper)
	{
		WallpaperHelper::registerHOOK();
		HWND parentHWND = WallpaperHelper::getWallpaperWindow();
		SetParent(winform->getHWND(), parentHWND);
	}
	else if (usage == Configuration::EngineUsage::AnimationRender)
	{
		ShowWindow(winform->getHWND(), SW_HIDE);
	}
}

void Aurora::iniRenderer(const UINT& msaaLevel, const UINT& screenWidth, const UINT& screenHeight)
{
	ID3D11DeviceContext4* ctx = nullptr;

	new Renderer(winform->getHWND(), screenWidth, screenHeight, msaaLevel, &ctx);

	new States();

	new Graphics(screenWidth, screenHeight, msaaLevel);

	new Camera();

	if (usage == Configuration::EngineUsage::AnimationRender)
	{
		encodeTexture = new Texture2D(screenWidth, screenHeight, 1, 1, FMT::BGRA8UN, D3D11_BIND_RENDER_TARGET, 0);

		new ImCtx(Graphics::getMSAALevel(), encodeTexture->getTexture2D());
	}
	else
	{
		new ImCtx(Graphics::getMSAALevel(), Renderer::instance->backBuffer.Get());
	}

	ImCtx::get()->imctx = ctx;

	TextureCube::iniShader();

#ifdef _DEBUG
	Renderer::getDevice()->QueryInterface(IID_ID3D11Debug, (void**)Renderer::instance->d3dDebug.ReleaseAndGetAddressOf());
#endif // _DEBUG

	if (enableImGui)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(winform->getHWND());
		ImGui_ImplDX11_Init(Renderer::getDevice(), ImCtx::GetContext());
	}
}

void Aurora::iniStates(const Configuration& config)
{
	//初始化一些默认状态，比如Viewport、BlendState等等 
	Camera::setProj(DirectX::XMMatrixOrthographicOffCenterLH(0.f, (float)Graphics::getWidth(), 0, (float)Graphics::getHeight(), -1.f, 1.f));

	ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

	ImCtx::get()->OMSetBlendState(States::defBlendState);

	ImCtx::get()->RSSetState(States::rasterCullBack);

	ImCtx::get()->OMSetDepthStencilState(States::defDepthStencilState, 0);

	ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);

	bindCommonCB();
}
