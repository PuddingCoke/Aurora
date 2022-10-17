#pragma once

#ifndef _COMPUTETEXTURE_H_
#define _COMPUTETEXTURE_H_

#include"ResourceTexture.h"

#include"DX/View/USView.h"

class ComputeTexture :public Texture2D, public USView
{
public:

	ComputeTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const unsigned int& extraBindFlags = 0);

	virtual ~ComputeTexture();

};

#endif // !_COMPUTETEXTURE_H_
