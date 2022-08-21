#pragma once

#ifndef _AURORA_H_
#define _AURORA_H_

//windows.h中也定义了min函数导致无法使用assimp https://github.com/assimp/assimp/issues/2271
#define NOMINMAX

#include<d3d11_4.h>
#include<chrono>
#include<iostream>
#include<windowsx.h>

#include"Camera.h"
#include"Graphics.h"
#include"Configuration.h"
#include"Game.h"
#include"Random.h"
#include"Keyboard.h"
#include"StateCommon.h"
#include"Shader.h"
#include"Math.h"
#include"TextureCube.h"

#include"VideoEncoder/NvidiaEncoder.h"
#include"Wallpaper/WallpaperHelper.h"

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

	HWND hwnd;

	ComPtr<ID3D11Texture2D> msaaTexture;

	ComPtr<ID3D11Texture2D> encodeTexture;

	ComPtr<IDXGISwapChain4> swapChain;

	const Configuration* config;

	Game* game;

	std::chrono::steady_clock timer;

	std::chrono::steady_clock::time_point timeStart;

	std::chrono::steady_clock::time_point timeEnd;

	HRESULT iniWindow();

	HRESULT iniDevice();

	HRESULT iniCamera();

	void runGame();

	void runEncode();

	//呼出控制台
	void allocateConsole();

	Aurora();

	static constexpr DWORD normalWndStyle = WS_CAPTION | WS_SYSMENU;

	static constexpr DWORD wallpaperWndStyle = WS_POPUP;

};

#endif // !_AURORA_H_
