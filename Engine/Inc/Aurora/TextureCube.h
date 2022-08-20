#pragma once

#ifndef _TEXTURECUBE_H_
#define _TEXTURECUBE_H_

#include"Texture2D.h"

class TextureCube
{
public:

	TextureCube() = delete;

	TextureCube(const TextureCube&) = delete;

	void operator=(const TextureCube&) = delete;

	//xp xn yp yn zp zn
	static TextureCube* create(std::initializer_list<std::string> texturesPath);

	void setSRV(const UINT& slot);

	~TextureCube();

private:

	TextureCube(std::initializer_list<std::string> texturesPath);

	ComPtr<ID3D11Texture2D> cubeTexture;
	
	ComPtr<ID3D11ShaderResourceView> cubeSRV;
};

#endif // !_TEXTURECUBE_H_
