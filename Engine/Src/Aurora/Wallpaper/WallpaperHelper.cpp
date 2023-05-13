#include<Aurora/Wallpaper/WallpaperHelper.h>

HHOOK WallpaperHelper::mouseHook;

void WallpaperHelper::getSystemResolution(UINT& width, UINT& height)
{
	HMONITOR monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);

	MONITORINFOEX monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	
	GetMonitorInfo(monitor, &monitorInfo);

	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmDriverExtra = 0;

	EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

	width = devMode.dmPelsWidth;
	height = devMode.dmPelsHeight;
}

HWND WallpaperHelper::getWallpaperWindow()
{
	HWND progman = FindWindow(L"ProgMan", nullptr);
	SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);
	HWND wallpaperHWND = nullptr;

	auto EnumWindowsProc = [](HWND hwnd, LPARAM lParam)->BOOL
	{
		HWND p = FindWindowEx(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);
		HWND* ret = (HWND*)lParam;
		if (p)
		{
			*ret = FindWindowEx(nullptr, hwnd, L"WorkerW", nullptr);
		}
		return true;
	};

	EnumWindows(EnumWindowsProc, (LPARAM)&wallpaperHWND);

	return wallpaperHWND;
}

void WallpaperHelper::registerHOOK()
{
	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, WallpaperHelper::MouseHookProc, NULL, 0);
}

void WallpaperHelper::unregisterHOOK()
{
	UnhookWindowsHookEx(mouseHook);
}

LRESULT CALLBACK WallpaperHelper::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

		switch (wParam)
		{
		case WM_MOUSEMOVE:
		{
			const float curX = (float)pMouseStruct->pt.x;
			const float curY = (float)Graphics::getHeight() - (float)pMouseStruct->pt.y;

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
		}

	}
	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}
