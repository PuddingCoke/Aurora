#pragma once

#ifndef _RESDEPTHTEXTURE_H_
#define _RESDEPTHTEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/DepthStencilView.h>
#include<Aurora/Core/DX/View/ShaderResourceView.h>

class ResDepthTexture :public Texture2D, public DepthStencilView, public ShaderResourceView
{
public:

	ResDepthTexture(const UINT& width, const UINT& height, const FMT& texFormat = FMT::R32TL, const FMT& srvFormat = FMT::R32F, const FMT& dsvFormat = FMT::D32F);

	virtual void bindDSV() override;

	virtual void bindSRV() override;

};

#endif // !_RESDEPTHTEXTURE_H_
