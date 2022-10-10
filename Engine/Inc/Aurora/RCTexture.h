#pragma once

#ifndef _RCTEXTURE_H_
#define _RCTEXTURE_H_

#include"Shader.h"
#include"ComputeTexture.h"
#include <DirectXColors.h>

class RCTexture :public ComputeTexture
{
public:

	RCTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4]);

	virtual ~RCTexture();

	void clearRTV(const float color[4]) const;

	void setRTV(ID3D11DepthStencilView* const view = nullptr) const;

private:

	ComPtr<ID3D11RenderTargetView> textureRTV;

};

#endif // !_RCTEXTURE_H_
