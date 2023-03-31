#pragma once

#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include"Graphics.h"
#include"DX/Resource/Texture2D.h"
#include"DX/View/Composition/RSView.h"
#include"RenderAPI.h"

#include<DirectXColors.h>

class RenderTexture :public Texture2D, public RSView
{
public:

	RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4] = DirectX::Colors::Black, const bool& enableMSAA = false);

	virtual ~RenderTexture();

	void resolve(Texture2D* const texture) const;

};

#endif // !_RENDERTEXTURE_H_
