#pragma once

#ifndef _DEPTHCUBE_H_
#define _DEPTHCUBE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/Composition/USView.h>
#include<Aurora/Core/DX/View/DepthStencilView.h>

class DepthCube :public Texture2D, public DepthStencilView, public ShaderResourceView
{
public:

	DepthCube(const UINT& resolution, const FMT& resFmt = FMT::R32TL, const FMT& srvFmt = FMT::R32F, const FMT& dsvFmt = FMT::D32F);

	virtual void bindDSV(ID3D11DeviceContext3* const ctx) override;

	virtual void bindSRV(ID3D11DeviceContext3* const ctx) override;

};

#endif // !_DEPTHCUBE_H_

