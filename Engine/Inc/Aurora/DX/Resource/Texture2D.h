#pragma once

#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include<string>
#include<d3d11_4.h>
#include<iostream>
#include<vector>

#include<Aurora/Utils.h>
#include<Aurora/Renderer.h>
#include<Aurora/Random.h>
#include<Aurora/Graphics.h>

#include<stb_image/stb_image.h>
#include<DDSTextureLoader/DDSTextureLoader11.h>

class Texture2D
{
public:

	enum class TextureType
	{
		Noise,
		Gauss
	};

	Texture2D() = delete;

	Texture2D(const Texture2D&) = delete;

	void operator=(const Texture2D&) = delete;

	Texture2D(const std::string& filePath, const D3D11_USAGE& usage = D3D11_USAGE_IMMUTABLE, const UINT& bindFlags = D3D11_BIND_SHADER_RESOURCE, const UINT& cpuAccessFlag = 0);

	Texture2D(const unsigned int& width, const unsigned int& height, const TextureType& type);

	Texture2D(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const D3D11_USAGE& usage, const UINT& bindFlags, const bool& enableMSAA = false, const UINT& cpuAccessFlag = 0);

	virtual ~Texture2D();

	const unsigned int& getWidth() const;

	const unsigned int& getHeight() const;

	const DXGI_FORMAT& getFormat() const;

	ID3D11Texture2D* getTexture2D() const;

protected:

	unsigned int width;

	unsigned int height;

	DXGI_FORMAT format;

	unsigned int mipLevels;

	ComPtr<ID3D11Texture2D> texture;
};

#endif // !_TEXTURE2D_H_
