#pragma once

#ifndef _DEPTHTEXTURE_H_
#define _DEPTHTEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/DepthStencilView.h>

class DepthTexture :public Texture2D, public DepthStencilView
{
public:

	DepthTexture(const UINT& width, const UINT& height, const FMT& fmt = FMT::D32F, const bool& enableMSAA = false);

};

#endif // !_DEPTHTEXTURE_H_
