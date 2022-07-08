#pragma once

#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include<DirectXColors.h>

#include"Texture2D.h"

class RenderTexture
{
public:

	RenderTexture() = delete;

	RenderTexture(const RenderTexture&) = delete;

	void operator=(const RenderTexture&) = delete;

	~RenderTexture();

	static RenderTexture* create(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const DirectX::XMVECTORF32& color = DirectX::Colors::Transparent);

	ComPtr<ID3D11RenderTargetView> msaaTarget;

	ComPtr<ID3D11RenderTargetView> normalTarget;

	void setMSAARTV(const UINT& slot, ID3D11DepthStencilView* const view = nullptr) const;

	void setRTV(const UINT& slot, ID3D11DepthStencilView* const view = nullptr) const;

	void resolve() const;

	Texture2D* getTexture() const;

	const unsigned int width;

	const unsigned int height;

	const DXGI_FORMAT format;

private:

	friend class SpriteBatch;

	RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const DirectX::XMVECTORF32& color);

	Texture2D* texture;

	ComPtr<ID3D11Texture2D> msaaTexture;

};

#endif // !_RENDERTEXTURE_H_
