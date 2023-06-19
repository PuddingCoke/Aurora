#pragma once

#ifndef _RESDEPTHTEXTURE_H_
#define _RESDEPTHTEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/DepthStencilView.h>
#include<Aurora/Core/DX/View/ShaderResourceView.h>

class ResourceDepthTexture :public Texture2D, public DepthStencilView, public ShaderResourceView
{
public:

	ResourceDepthTexture(const UINT& width, const UINT& height, const FMT& resFmt = FMT::R32TL, const FMT& srvFmt = FMT::R32F, const FMT& dsvFmt = FMT::D32F);

	virtual void bindDSV(ID3D11DeviceContext3* const ctx) override;

	virtual void bindSRV(ID3D11DeviceContext3* const ctx) override;

};

#endif // !_RESDEPTHTEXTURE_H_
