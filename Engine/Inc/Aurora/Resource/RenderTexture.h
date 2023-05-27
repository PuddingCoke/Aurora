#pragma once

#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/Composition/RSView.h>

class RenderTexture :public Texture2D, public RSView
{
public:

	RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4] = DirectX::Colors::Black, const bool& enableMSAA = false);

	virtual ~RenderTexture();

};

#endif // !_RENDERTEXTURE_H_
