#pragma once

#ifndef _COMPUTETEXTURE3D_H_
#define _COMPUTETEXTURE3D_H_

#include"DX/Resource/Texture3D.h"
#include"DX/View/Composition/USView.h"

class ComputeTexture3D :public Texture3D, public USView
{
public:

	ComputeTexture3D(const UINT& width, const UINT& height, const UINT& depth, const DXGI_FORMAT& format, const UINT& extraBindFlags = 0, const UINT& miscFlags = 0, const UINT& mipLevels = 1);

	ComputeTexture3D(const UINT& width, const UINT& height, const UINT& depth, const DXGI_FORMAT& resFmt, const DXGI_FORMAT& srvFmt, const DXGI_FORMAT& uavFmt, const UINT& extraBindFlags = 0, const UINT& miscFlags = 0, const UINT& mipLevels = 1);

	virtual ~ComputeTexture3D();

};

#endif // !_COMPUTETEXTURE3D_H_
