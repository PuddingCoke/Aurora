#include<Aurora/Wallpaper/WallpaperHelper.h>

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
