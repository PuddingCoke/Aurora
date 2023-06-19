#include<Aurora/Resource/ResourceTexture.h>

ResourceTexture::ResourceTexture(const std::string& filePath) :
	Texture2D(filePath)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = mipLevels;

	createSRV(texture.Get(), srvDesc);
}

ResourceTexture::ResourceTexture(const UINT& width, const UINT& height, const TextureType& type) :
	Texture2D(width, height, type)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = mipLevels;

	createSRV(texture.Get(), srvDesc);
}

ResourceTexture::ResourceTexture(const UINT& width, const UINT& height, const FMT& fmt, const bool& enableMSAA) :
	Texture2D(width, height, fmt, D3D11_BIND_SHADER_RESOURCE, enableMSAA)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = FMTCAST(fmt);

	if (enableMSAA)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = mipLevels;
	}

	createSRV(texture.Get(), srvDesc);
}

ResourceTexture::~ResourceTexture()
{
}

void ResourceTexture::bindSRV(ID3D11DeviceContext3* const ctx)
{
}
