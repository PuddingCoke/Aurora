#pragma once

#ifndef _WALLPAPERHELPER_H_
#define _WALLPAPERHELPER_H_

#define NOMINMAX

#include<Windows.h>

#include<Aurora/Graphics.h>
#include<Aurora/Mouse.h>

class WallpaperHelper
{
public:

	WallpaperHelper() = delete;

	WallpaperHelper(const WallpaperHelper&) = delete;

	void operator=(const WallpaperHelper&) = delete;

	static void getSystemResolution(int& width, int& height);

	static HWND getWallpaperWindow();

	static void registerHOOK();

	static void unregisterHOOK();

	static LRESULT WINAPI MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

	//鼠标全局钩子
	static HHOOK mouseHook;

};

#endif // !_WALLPAPERHELPER_H_
