#pragma once

#ifndef _WALLPAPERHELPER_H_
#define _WALLPAPERHELPER_H_

#include<Windows.h>
#include<iostream>

class WallpaperHelper
{
public:

	static void getSystemResolution(int& width, int& height);

	static HWND getWallpaperWindow();

	WallpaperHelper() = delete;

	WallpaperHelper(const WallpaperHelper&) = delete;

	void operator=(const WallpaperHelper&) = delete;

};

#endif // !_WALLPAPERHELPER_H_
