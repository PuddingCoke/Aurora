#pragma once

#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include<DirectXColors.h>
#include<iterator>

#include"Texture2D.h"

class RenderTexture
{
public:

	RenderTexture() = delete;

	RenderTexture(const RenderTexture&) = delete;

	void operator=(const RenderTexture&) = delete;

	~RenderTexture();

	static RenderTexture* create(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4] = DirectX::Colors::Transparent, const bool& enableMSAA = true);

	void setMSAARTV(ID3D11DepthStencilView* const view = nullptr) const;

	void setRTV(ID3D11DepthStencilView* const view = nullptr) const;

	void clearMSAARTV(const float color[4]);

	void clearRTV(const float color[4]);

	void resolve() const;

	Texture2D* getTexture() const;

	const unsigned int width;

	const unsigned int height;

	const DXGI_FORMAT format;

	static void setRTVs(std::initializer_list<RenderTexture*> renderTextures, ID3D11DepthStencilView* view = nullptr);

	static void setMSAARTVs(std::initializer_list<RenderTexture*> renderTextures, ID3D11DepthStencilView* msaaView = nullptr);

	static void unbindAll();

private:
	
	static ID3D11RenderTargetView* renderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4],const bool& enableMSAA);

	Texture2D* const texture;

	ComPtr<ID3D11Texture2D> msaaTexture;

	ComPtr<ID3D11RenderTargetView> msaaTarget;

	ComPtr<ID3D11RenderTargetView> normalTarget;

};

#endif // !_RENDERTEXTURE_H_
