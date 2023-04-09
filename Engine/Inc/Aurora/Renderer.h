#pragma once

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include<d3d11_4.h>
#include<wrl/client.h>
#include<DirectXMath.h>
#include<iostream>

using Microsoft::WRL::ComPtr;

enum class GPUManufacturer
{
	NVIDIA,
	AMD,
	INTEL
};

class Renderer
{
public:

	Renderer() = delete;

	Renderer(const Renderer&) = delete;

	void operator=(const Renderer&) = delete;

	static ID3D11Device5* device;

	static ID3D11DeviceContext4* getContext();

	static const GPUManufacturer& getGPUManufacturer();

private:

	friend class Aurora;

	friend class Shader;

	friend class ResManager;

	friend class RenderAPI;

	static Renderer* instance;

	static ID3D11DeviceContext4* context;

	Renderer(HWND hWnd, const unsigned int& width, const unsigned int& height, const bool& enableDebug, const unsigned int& msaaLevel, const UINT& extraDeviceFlags = 0);

	D3D11_VIEWPORT vp;

	ComPtr<ID3D11Device5> device5;

	ComPtr<ID3D11DeviceContext4> context4;

	ComPtr<IDXGISwapChain4> swapChain;

	ComPtr<ID3D11Debug> d3dDebug;

	ComPtr<ID3D11Texture2D> backBuffer;

	ComPtr<ID3D11Texture2D> msaaTexture;

	GPUManufacturer gpuManufacturer;

};

#endif // !_RENDERER_H_