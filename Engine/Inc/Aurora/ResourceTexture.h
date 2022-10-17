#pragma once

#ifndef _RESOURCETEXTURE_H_
#define _RESOURCETEXTURE_H_

#include"DX/Resource/Texture2D.h"
#include"DX/View/ShaderResourceView.h"

class ResourceTexture :public Texture2D, public ShaderResourceView
{
public:

	ResourceTexture() = delete;

	ResourceTexture(const ResourceTexture&) = delete;

	void operator=(const ResourceTexture&) = delete;

	ResourceTexture(const std::string& filePath, const D3D11_USAGE& usage = D3D11_USAGE_IMMUTABLE, const UINT& extraBindFlags = 0, const UINT& cpuAccessFlag = 0);

	ResourceTexture(const unsigned int& width, const unsigned int& height, const TextureType& type);

	ResourceTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const D3D11_USAGE& usage, const UINT& extraBindFlags = 0, const bool& enableMSAA = false, const UINT& cpuAccessFlag = 0);

	virtual ~ResourceTexture();

private:

	void createShaderResource();

};

#endif // !_RESOURCETEXTURE_H_
