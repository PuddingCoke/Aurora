#pragma once

#ifndef _COMPUTETEXTURE_H_
#define _COMPUTETEXTURE_H_

#include"DX/Resource/Texture2D.h"
#include"DX/View/USView.h"

class ComputeTexture :public Texture2D, public USView
{
public:

	ComputeTexture(const UINT& width, const UINT& height, const DXGI_FORMAT& format, const UINT& arraySize = 1);

	ComputeTexture(const UINT& width, const UINT& height, const DXGI_FORMAT& resFmt, const DXGI_FORMAT& srvFmt, const DXGI_FORMAT& uavFmt, const UINT& arraySize = 1);

	virtual ~ComputeTexture();

};

#endif // !_COMPUTETEXTURE_H_
