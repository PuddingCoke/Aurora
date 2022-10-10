#pragma once

#ifndef _COMPUTETEXTURE_H_
#define _COMPUTETEXTURE_H_

#include"Texture2D.h"

class ComputeTexture :public Texture2D
{
public:

	ComputeTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const unsigned int& extraBindFlags = 0);

	void CSSetUAV(const UINT& slot = 0) const;

	virtual ~ComputeTexture();

protected:

	ComPtr<ID3D11UnorderedAccessView> textureUAV;

};

#endif // !_COMPUTETEXTURE_H_
