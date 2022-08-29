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

class Texture2D
{
public:

	Texture2D() = delete;

	Texture2D(const Texture2D&) = delete;

	void operator=(const Texture2D&) = delete;

	static Texture2D* create(const std::string& path);

	static Texture2D* create(const std::string& path, const D3D11_USAGE& usage, const UINT& bindFlag, const UINT& cpuAccessFlag);
	
	static Texture2D* create(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const D3D11_USAGE& usage, const UINT& bindFlags, const UINT& cpuAccessFlag);

	//生成一个噪音贴图 每个通道属于[0,1]
	static Texture2D* createNoise(const unsigned int& width, const unsigned int& height);

	~Texture2D();

	const unsigned int& getWidth() const;

	const unsigned int& getHeight() const;

	const DXGI_FORMAT& getFormat() const;

	void setSRV(const UINT& slot = 0) const;

	ID3D11ShaderResourceView* getSRV() const;

	ID3D11Texture2D* getTexture2D() const;

private:

	friend class SpriteBatch;

	Texture2D(const std::string& path, const D3D11_USAGE& usage, const UINT& bindFlag, const UINT& cpuAccessFlag);

	Texture2D(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const D3D11_USAGE& usage, const UINT& bindFlags, const UINT& cpuAccessFlag);

	Texture2D(const unsigned int& width, const unsigned int& height);

	void createShaderResource();

	ComPtr<ID3D11ShaderResourceView> resourceView;

	ComPtr<ID3D11Texture2D> texture2D;

	unsigned int width;

	unsigned int height;

	int poolIndex;

	DXGI_FORMAT format;

};

using Texture2DPtr = std::shared_ptr<Texture2D>;

#endif // !_TEXTURE2D_H_
