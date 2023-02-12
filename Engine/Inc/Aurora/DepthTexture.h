#pragma once

#ifndef _DEPTHTEXTURE_H_
#define _DEPTHTEXTURE_H_

#include<Aurora/DX/Resource/Texture2D.h>
#include<Aurora/DX/View/DepthStencilView.h>

class DepthTexture :public Texture2D, public DepthStencilView
{
public:

	DepthTexture(const UINT& width, const UINT& height, const DXGI_FORMAT& format = DXGI_FORMAT_D32_FLOAT, const bool& enableMSAA = false);

};

#endif // !_DEPTHTEXTURE_H_
