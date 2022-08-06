#pragma once

#ifndef _DEPTHSTENCILVIEW_H_
#define _DEPTHSTENCILVIEW_H_

#include"Graphics.h"

class DepthStencilView
{
public:

	DepthStencilView() = delete;

	DepthStencilView(const DepthStencilView&) = delete;

	void operator=(const DepthStencilView&) = delete;

	static DepthStencilView* create(const DXGI_FORMAT& format,const bool& enableMSAA = true);

	void clear(const UINT& clearFlag, const float& depth = 1.0f, const UINT8& stencil = 0) const;

	ID3D11DepthStencilView* get() const;

	~DepthStencilView();

private:

	DepthStencilView(const DXGI_FORMAT& format, const bool& enableMSAA);

	ComPtr<ID3D11Texture2D> depthStencilTexture;

	ComPtr<ID3D11DepthStencilView> depthStencilView;

};

#endif // !_DEPTHSTENCILVIEW_H_
