#pragma once

#ifndef _RESDEPTHTEXTURE_H_
#define _RESDEPTHTEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/DepthStencilView.h>
#include<Aurora/Core/DX/View/ShaderResourceView.h>

class ResDepthTexture :public Texture2D, public DepthStencilView, public ShaderResourceView
{
public:

	ResDepthTexture(const UINT& width, const UINT& height, const DXGI_FORMAT& texFormat = DXGI_FORMAT_R32_TYPELESS, const DXGI_FORMAT& srvFormat = DXGI_FORMAT_R32_FLOAT, const DXGI_FORMAT& dsvFormat = DXGI_FORMAT_D32_FLOAT);

	virtual void bindDSV() override;

	virtual void bindSRV() override;

};

#endif // !_RESDEPTHTEXTURE_H_
