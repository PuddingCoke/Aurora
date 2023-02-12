#pragma once

#ifndef _COMPUTETEXTURE_H_
#define _COMPUTETEXTURE_H_

#include"DX/Resource/Texture2D.h"
#include"DX/View/USView.h"

class ComputeTexture :public Texture2D, public USView
{
public:

	ComputeTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format);

	virtual ~ComputeTexture();

};

#endif // !_COMPUTETEXTURE_H_
