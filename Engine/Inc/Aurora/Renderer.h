#pragma once

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include<d3d11_4.h>
#include<wrl/client.h>
#include<DirectXMath.h>
#include<iostream>

using Microsoft::WRL::ComPtr;

class Renderer
{
public:

	Renderer() = delete;

	Renderer(const Renderer&) = delete;

	void operator=(const Renderer&) = delete;

	static ID3D11Device4* device;

	static ID3D11DeviceContext4* context;

	static void setDefRTV(ID3D11DepthStencilView* const view = nullptr);

	static void clearDefRTV(const float* color);

	static void setViewport(const float& width, const float& height);

	static void setViewport(const unsigned int& width, const unsigned int& height);

	static void setViewport(const int& width, const int& height);

	static void setTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology);

	static void setBlendState(ID3D11BlendState* const blendState);

	static void setRasterizerState(ID3D11RasterizerState* const state);

	static void drawQuad();

	static void drawCube();

	static void draw(const UINT& vertexCount, const UINT& startVertexLocation);

	static void drawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation);

	static void drawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation);

private:

	friend class Aurora;

	static Renderer* instance;

	Renderer(HWND hWnd, const unsigned int& width, const unsigned int& height, const bool& enableDebug, const unsigned int& msaaLevel);

	D3D11_VIEWPORT vp;

	ComPtr<ID3D11Device4> device4;

	ComPtr<ID3D11DeviceContext4> context4;

	ComPtr<IDXGISwapChain4> swapChain;

	ComPtr<ID3D11Debug> d3dDebug;

	ComPtr<ID3D11Texture2D> backBuffer;

	ComPtr<ID3D11Texture2D> msaaTexture;

	ComPtr<ID3D11RenderTargetView> defaultTargetView;

};

#endif // !_RENDERER_H_