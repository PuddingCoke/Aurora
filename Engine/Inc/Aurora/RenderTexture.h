#pragma once

#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include<DirectXColors.h>
#include<iterator>

#include"Texture2D.h"
#include"Graphics.h"

class RenderTexture :public Texture2D
{
public:

	RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4] = DirectX::Colors::Black, const bool& enableMSAA = false);

	virtual ~RenderTexture();

	void setMSAARTV(ID3D11DepthStencilView* const view = nullptr) const;

	ID3D11RenderTargetView* getRTV() const;

	void setRTV(ID3D11DepthStencilView* const view = nullptr) const;

	void clearMSAARTV(const float color[4]) const;

	void clearRTV(const float color[4]) const;

	void resolve() const;

	static void setRTVs(std::initializer_list<RenderTexture*> renderTextures, ID3D11DepthStencilView* view = nullptr);

	static void setMSAARTVs(std::initializer_list<RenderTexture*> renderTextures, ID3D11DepthStencilView* msaaView = nullptr);

	static void unbindAll();

private:

	static ID3D11RenderTargetView* renderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	ComPtr<ID3D11Texture2D> msaaTexture;

	ComPtr<ID3D11RenderTargetView> msaaTarget;

	ComPtr<ID3D11RenderTargetView> normalTarget;

};

#endif // !_RENDERTEXTURE_H_
