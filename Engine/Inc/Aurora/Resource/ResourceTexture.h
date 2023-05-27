#pragma once

#ifndef _RESOURCETEXTURE_H_
#define _RESOURCETEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/ShaderResourceView.h>

class ResourceTexture :public Texture2D, public ShaderResourceView
{
public:

	ResourceTexture() = delete;

	ResourceTexture(const ResourceTexture&) = delete;

	void operator=(const ResourceTexture&) = delete;

	ResourceTexture(const std::string& filePath);

	ResourceTexture(const unsigned int& width, const unsigned int& height, const TextureType& type);

	ResourceTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const bool& enableMSAA = false);

	virtual ~ResourceTexture();

	virtual void bindSRV() override;

};

#endif // !_RESOURCETEXTURE_H_
