#pragma once

#ifndef _DEPTHCUBE_H_
#define _DEPTHCUBE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/Composition/USView.h>
#include<Aurora/Core/DX/View/DepthStencilView.h>

class DepthCube :public Texture2D, public DepthStencilView, public ShaderResourceView
{
public:

	DepthCube(const UINT& resolution, const FMT& texFmt = FMT::R32TL, const FMT& srvFmt = FMT::R32F, const FMT& dsvFmt = FMT::D32F);

	virtual void bindDSV() override;

	virtual void bindSRV() override;

};

#endif // !_DEPTHCUBE_H_

