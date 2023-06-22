#pragma once

#ifndef _AURORA_H_
#define _AURORA_H_

//windows.h中也定义了min函数导致无法使用assimp https://github.com/assimp/assimp/issues/2271
#define NOMINMAX

#include"Window/Win32Form.h"

#include<iostream>
#include<chrono>
#include<future>
#include<windowsx.h>

#include<ImGUI/imgui.h>
#include<ImGUI/imgui_impl_win32.h>
#include<ImGUI/imgui_impl_dx11.h>

#include<Aurora/Game.h>
#include<Aurora/Configuration.h>

#include<Aurora/Core/GraphicsDevice.h>

#include<Aurora/EngineAPI/Camera.h>
#include<Aurora/EngineAPI/Graphics.h>
#include<Aurora/EngineAPI/States.h>
#include<Aurora/EngineAPI/ImCtx.h>
#include<Aurora/EngineAPI/BufferUpdate.h>

#include<Aurora/VideoEncoder/NvidiaEncoder.h>
#include<Aurora/Wallpaper/WallpaperHelper.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum class GPUManufacturer
{
	NVIDIA,
	AMD,
	INTEL,
	UNKNOWN
};

class Aurora
{
public:

	Aurora(const Aurora&) = delete;

	void operator=(const Aurora&) = delete;

	static Aurora& get();

	int iniEngine(const Configuration& config, const int& argc, const char* argv[]);

	void iniGame(Game* const game);

private:

	static Aurora instance;

	GPUManufacturer manufacturer;

	Win32Form* winform;

	Game* game;

	Configuration::EngineUsage usage;

	bool enableImGui;

	Texture2D* encodeTexture;

	ComPtr<IDXGISwapChain4> swapChain;

	ComPtr<ID3D11Debug> d3dDebug;

	ComPtr<ID3D11Texture2D> backBuffer;

	ComPtr<ID3D11Texture2D> msaaTexture;

	void runGame();

	void runEncode();

	void bindCommonCB();

	void destroy();

	Aurora();

	void iniWindow(const std::wstring& title, const UINT& screenWidth, const UINT& screenHeight);

	void iniRenderer(const UINT& msaaLevel, const UINT& screenWidth, const UINT& screenHeight);

	void iniStates(const Configuration& config);

	static constexpr DWORD normalWndStyle = WS_CAPTION | WS_SYSMENU;

	static constexpr DWORD wallpaperWndStyle = WS_POPUP;

	static constexpr FMT backBufferFmt = FMT::BGRA8UN;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WallpaperProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

};

#endif // !_AURORA_H_
