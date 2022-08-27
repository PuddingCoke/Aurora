﻿#include<Aurora/Wallpaper/WallpaperHelper.h>

HHOOK WallpaperHelper::mouseHook;

void WallpaperHelper::getSystemResolution(int& width, int& height)
{
	HMONITOR monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
	MONITORINFO info = {};
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	width = info.rcMonitor.right - info.rcMonitor.left;
	height = info.rcMonitor.bottom - info.rcMonitor.top;

	//以上代码获取的width和height实际上是 windowWidth/dpi*96 windowHeight/dpi*96所以还得获取屏幕dpi

	UINT dpi = GetDpiForWindow(GetDesktopWindow());

	std::cout << "[class WallpaperHelper] system dpi " << dpi << "\n"; 

	width = width * dpi / 96;
	height = height * dpi / 96;
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
	auto mouseProc = [](int nCode, WPARAM wParam, LPARAM lParam)
	{
		return MouseHookProc(nCode, wParam, lParam);
	};

	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, NULL, 0);
}

void WallpaperHelper::unregisterHOOK()
{
	UnhookWindowsHookEx(mouseHook);
}

LRESULT __stdcall WallpaperHelper::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
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

		case WM_MOUSEWHEEL:
			Mouse::wheelDelta = (float)GET_WHEEL_DELTA_WPARAM(wParam) / 120.f;
			Mouse::scrollEvent();
			break;
		}

	}
	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}
