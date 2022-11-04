﻿#pragma once

#ifndef _DEPTHSTENCILVIEW_H_
#define _DEPTHSTENCILVIEW_H_

#include<Aurora/Graphics.h>

class DepthStencilView
{
public:

	DepthStencilView(const DepthStencilView&) = delete;

	void operator=(const DepthStencilView&) = delete;

	DepthStencilView(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const bool& enableMSAA = false);

	void clear(const UINT& clearFlag, const float& depth = 1.0f, const UINT8& stencil = 0) const;

	ID3D11DepthStencilView* get() const;

	~DepthStencilView();

	virtual void bindDSV();

protected:

	DepthStencilView();

	ComPtr<ID3D11Texture2D> depthStencilTexture;

	ComPtr<ID3D11DepthStencilView> depthStencilView;

};

#endif // !_DEPTHSTENCILVIEW_H_
