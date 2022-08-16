#pragma once

#ifndef _WALLPAPERHELPER_H_
#define _WALLPAPERHELPER_H_

#include<Windows.h>
#include<iostream>

#include<Aurora/Graphics.h>
#include<Aurora/Mouse.h>

class WallpaperHelper
{
public:

	static void getSystemResolution(int& width, int& height);

	static HWND getWallpaperWindow();

	static void registerHOOK();

	static void unregisterHOOK();

	WallpaperHelper() = delete;

	WallpaperHelper(const WallpaperHelper&) = delete;

	void operator=(const WallpaperHelper&) = delete;

	static LRESULT WINAPI MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

	//���ȫ�ֹ���
	static HHOOK mouseHook;

};

#endif // !_WALLPAPERHELPER_H_
