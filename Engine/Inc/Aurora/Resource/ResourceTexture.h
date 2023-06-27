#pragma once

#ifndef _RESOURCETEXTURE_H_
#define _RESOURCETEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/ShaderResourceView.h>

class ResourceTexture :public Texture2D, public ShaderResourceView
{
public:

	ResourceTexture(const std::string& filePath);

	ResourceTexture(const UINT& width, const UINT& height, const TextureType& type);

	ResourceTexture(const UINT& width, const UINT& height, const FMT& fmt, const bool& enableMSAA = false);

	virtual ~ResourceTexture();

	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

};

#endif // !_RESOURCETEXTURE_H_
