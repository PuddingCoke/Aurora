#pragma once

#ifndef _TEXTURE3D_H_
#define _TEXTURE3D_H_

#include"Resource.h"

class Texture3D :public Resource
{
public:

	virtual ~Texture3D();

	Texture3D() = delete;

	Texture3D(const Texture3D&);

	Texture3D(const UINT& width, const UINT& height, const UINT& depth, const FMT& format, const UINT& bindFlags, const UINT& miscFlags = 0, const UINT& mipLevels = 1);

	UINT width;

	UINT height;

	UINT depth;

	UINT mipLevels;

	ID3D11Resource* getResource() const override;

	ID3D11Texture3D* getTexture3D() const;

protected:

	ComPtr<ID3D11Texture3D> texture;

};

#endif // !_TEXTURE3D_H_
