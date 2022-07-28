﻿#pragma once

#ifndef _AURORA_H_
#define _AURORA_H_

#include<Windows.h>
#include<d3d11_4.h>
#include<chrono>
#include<iostream>
#include<windowsx.h>

#include"Graphics.h"
#include"Configuration.h"
#include"Game.h"
#include"Random.h"
#include"Mouse.h"
#include"Keyboard.h"
#include"StateCommon.h"
#include"Shader.h"
#include"Math.h"

class Aurora
{
public:

	Aurora(const Aurora&) = delete;

	void operator=(const Aurora&) = delete;

	static Aurora& get();

	int iniEngine(const Configuration& config);

	void iniGame(Game* const game);

	LRESULT WINAPI WallpaperMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK WallpaperProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	static Aurora instance;

	HWND hwnd;

	ComPtr<ID3D11Texture2D> msaaTexture;

	ID3D11Texture2D* backBuffer;

	ComPtr<IDXGISwapChain4> swapChain;

	const Configuration* config;

	Game* game;

	std::chrono::steady_clock timer;

	std::chrono::steady_clock::time_point timeStart;

	std::chrono::steady_clock::time_point timeEnd;

	HRESULT iniWindow();

	HRESULT iniDevice();

	HRESULT iniGameConstant();

	void runGame();

	Aurora();

	//这个钩子是用于动态壁纸的
	HHOOK mouseHook;

	static constexpr DWORD normalWndStyle = WS_CAPTION | WS_SYSMENU;

	static constexpr DWORD wallpaperWndStyle = WS_POPUP;

};

#endif // !_AURORA_H_
