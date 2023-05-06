#pragma once

#ifndef _TEXTURE3D_H_
#define _TEXTURE3D_H_

#include<Aurora/Renderer.h>

#include"Resource.h"

class Texture3D :public Resource
{
public:

	Texture3D() = delete;

	Texture3D(const Texture3D&) = delete;

	void operator=(const Texture3D&) = delete;

	virtual ~Texture3D();

	Texture3D(const UINT& width, const UINT& height, const UINT& depth, const DXGI_FORMAT& format, const UINT& bindFlags, const UINT& miscFlags = 0, const UINT& mipLevels = 1);

	const UINT width;

	const UINT height;

	const UINT depth;

	ID3D11Resource* getResource() const override;

	ID3D11Texture3D* getTexture3D() const;

protected:

	ComPtr<ID3D11Texture3D> texture;

};

#endif // !_TEXTURE3D_H_
