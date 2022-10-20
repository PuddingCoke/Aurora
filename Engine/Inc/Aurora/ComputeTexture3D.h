#pragma once

#ifndef _COMPUTETEXTURE3D_H_
#define _COMPUTETEXTURE3D_H_

#include"DX/Resource/Texture3D.h"
#include"DX/View/USView.h"

class ComputeTexture3D :public Texture3D, public USView
{
public:

	ComputeTexture3D(const UINT& width, const UINT& height, const UINT& depth, const DXGI_FORMAT& format, const UINT& extraBindFlags = 0);

	virtual ~ComputeTexture3D();

};

#endif // !_COMPUTETEXTURE3D_H_
