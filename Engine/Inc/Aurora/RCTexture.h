#pragma once

#ifndef _RCTEXTURE_H_
#define _RCTEXTURE_H_

#include"DX/Resource/Texture2D.h"
#include"DX/View/Composition/RUSView.h"

class RCTexture :public Texture2D, public RUSView
{
public:

	RCTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4]);

	virtual ~RCTexture();

};

#endif // !_RCTEXTURE_H_
