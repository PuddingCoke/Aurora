#pragma once

#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include<string>
#include<d3d11_4.h>
#include<memory>
#include<iostream>

#include<stb_image/stb_image.h>
#include<DDSTextureLoader/DDSTextureLoader11.h>

#include"Renderer.h"
#include"Utils.h"
#include"Random.h"

#include"DX/ShaderResourceView.h"

class Texture2D :public ShaderResourceView
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

	//读取文件
	Texture2D(const std::string& path, const D3D11_USAGE& usage = D3D11_USAGE_DEFAULT, const UINT& bindFlag = D3D11_BIND_SHADER_RESOURCE, const UINT& cpuAccessFlag = 0);

	//创建自定义材质
	Texture2D(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const D3D11_USAGE& usage, const UINT& bindFlags, const UINT& cpuAccessFlag = 0);

	//生成噪音材质、随机高斯分布材质
	Texture2D(const unsigned int& width, const unsigned int& height, const TextureType& type);

	virtual ~Texture2D();

	const unsigned int& getWidth() const;

	const unsigned int& getHeight() const;

	const DXGI_FORMAT& getFormat() const;

	ID3D11Texture2D* getTexture2D() const;

protected:

	ComPtr<ID3D11Texture2D> texture;

	unsigned int width;

	unsigned int height;

	DXGI_FORMAT format;

};

using Texture2DPtr = std::shared_ptr<Texture2D>;

#endif // !_TEXTURE2D_H_
