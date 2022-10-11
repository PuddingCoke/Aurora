#pragma once

#ifndef _COMPUTETEXTURE_H_
#define _COMPUTETEXTURE_H_

#include"Texture2D.h"

#include"DX/UnorderedAccessView.h"

class ComputeTexture :public Texture2D, public UnorderedAccessView
{
public:

	ComputeTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const unsigned int& extraBindFlags = 0);

	virtual ~ComputeTexture();

};

#endif // !_COMPUTETEXTURE_H_
