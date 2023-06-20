#pragma once

#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include<string>
#include<d3d11_4.h>
#include<iostream>
#include<vector>

#include<Aurora/Utils/Utils.h>
#include<Aurora/Utils/Random.h>
#include<Aurora/EngineAPI/Graphics.h>

#include<stb_image/stb_image.h>
#include<DDSTextureLoader/DDSTextureLoader11.h>

#include"Resource.h"

class Texture2D :public Resource
{
public:

	enum class TextureType
	{
		Noise,
		Gauss
	};

	Texture2D();

	Texture2D(const Texture2D&) = delete;

	void operator=(const Texture2D&) = delete;

	Texture2D(const std::string& filePath);

	Texture2D(const UINT& width, const UINT& height, const TextureType& type);

	Texture2D(const UINT& width, const UINT& height, const FMT& format, const UINT& bindFlags, const bool& enableMSAA = false);

	Texture2D(const UINT& width, const UINT& height, const UINT& mipLevels, const UINT& arraySize, const FMT& format, const UINT& bindFlags, const UINT& miscFlags);

	virtual ~Texture2D();

	const UINT& getWidth() const;

	const UINT& getHeight() const;

	const DXGI_FORMAT& getFormat() const;

	const UINT& getMipLevels() const;

	const UINT& getArraySize() const;

	ID3D11Resource* getResource() const override;

	ID3D11Texture2D* getTexture2D() const;

protected:

	UINT width;

	UINT height;

	DXGI_FORMAT format;

	UINT mipLevels;

	UINT arraySize;

	ComPtr<ID3D11Texture2D> texture;
};

#endif // !_TEXTURE2D_H_
