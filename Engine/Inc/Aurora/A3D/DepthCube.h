#pragma once

#ifndef _DEPTHCUBE_H_
#define _DEPTHCUBE_H_

#include<Aurora/DX/Resource/Texture2D.h>
#include<Aurora/DX/View/Composition/USView.h>
#include<Aurora/DX/View/DepthStencilView.h>

class DepthCube :public Texture2D, public DepthStencilView, public ShaderResourceView
{
public:

	DepthCube(const UINT& resolution, const DXGI_FORMAT& texFormat = DXGI_FORMAT_R32_TYPELESS, const DXGI_FORMAT& srvFormat = DXGI_FORMAT_R32_FLOAT, const DXGI_FORMAT& dsvFormat = DXGI_FORMAT_D32_FLOAT);

	virtual void bindDSV() override;

};

#endif // !_DEPTHCUBE_H_

