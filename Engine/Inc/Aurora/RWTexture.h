#pragma once

#ifndef _RWTEXTURE_H_
#define _RWTEXTURE_H_

#include"Shader.h"
#include"Texture2D.h"
#include <DirectXColors.h>

class RWTexture
{
public:

	RWTexture() = delete;

	RWTexture(const RWTexture&) = delete;

	void operator=(const RWTexture&) = delete;

	static RWTexture* create(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4] = DirectX::Colors::Black);

	~RWTexture();

	const unsigned int width;

	const unsigned int height;

	const DXGI_FORMAT format;

	void clearRTV(const float color[4]) const;

	void setRTV(ID3D11DepthStencilView* const view = nullptr) const;

	void PSSetSRV(const UINT& slot = 0) const;

	void CSSetSRV(const UINT& slot = 0) const;

	void setUAV(const UINT& slot = 0) const;

private:

	RWTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4]);

	Texture2D* const texture;

	ComPtr<ID3D11RenderTargetView> textureRTV;

	ComPtr<ID3D11UnorderedAccessView> textureUAV;

};

#endif // !_RWTEXTURE_H_
