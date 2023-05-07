#pragma once

#ifndef _AURORA_H_
#define _AURORA_H_

//windows.h中也定义了min函数导致无法使用assimp https://github.com/assimp/assimp/issues/2271
#define NOMINMAX

#include"Window/Win32Form.h"

#include<chrono>
#include<iostream>
#include<windowsx.h>

#include<ImGUI/imgui.h>
#include<ImGUI/imgui_impl_win32.h>
#include<ImGUI/imgui_impl_dx11.h>

#include"Camera.h"
#include"Graphics.h"
#include"Configuration.h"
#include"Game.h"
#include"Keyboard.h"
#include"States.h"
#include"Math.h"
#include"ResManager.h"
#include"RenderAPI.h"
#include"Utils.h"
#include"PerframeCB.h"
#include"A3D/TextureCube.h"

#include"VideoEncoder/NvidiaEncoder.h"
#include"Wallpaper/WallpaperHelper.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Aurora
{
public:

	Aurora(const Aurora&) = delete;

	void operator=(const Aurora&) = delete;

	static Aurora& get();

	int iniEngine(const Configuration& config);

	void iniGame(Game* const game);

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK WallpaperProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	static Aurora instance;

	Win32Form* winform;

	Game* game;

	Configuration::EngineUsage usage;

	bool enableDebug;

	bool enableImGui;

	Texture2D* encodeTexture;

	void runGame();

	void runEncode();

	void bindCommonCB();

	void destroy();

	Aurora();

	void iniWindow(const std::wstring& title,const UINT& screenWidth, const UINT& screenHeight);

	void iniRenderer(const UINT& msaaLevel, const UINT& screenWidth, const UINT& screenHeight);

	void iniStates(const Configuration& config);

	static constexpr DWORD normalWndStyle = WS_CAPTION | WS_SYSMENU;

	static constexpr DWORD wallpaperWndStyle = WS_POPUP;

};

#endif // !_AURORA_H_
