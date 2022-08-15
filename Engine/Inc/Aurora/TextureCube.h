#pragma once

#ifndef _TEXTURECUBE_H_
#define _TEXTURECUBE_H_

#include"Graphics.h"

class TextureCube
{
public:

	void setSRV(const UINT& slot);

private:

	ComPtr<ID3D11Texture2D> cubeTexture;
	
	ComPtr<ID3D11ShaderResourceView> cubeSRV;
};

#endif // !_TEXTURECUBE_H_
