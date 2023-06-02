#pragma once

#ifndef _RCTEXTURE_H_
#define _RCTEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/Composition/RUSView.h>

class RenderComputeTexture :public Texture2D, public RUSView
{
public:

	RenderComputeTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4]);

	virtual ~RenderComputeTexture();

};

#endif // !_RCTEXTURE_H_
