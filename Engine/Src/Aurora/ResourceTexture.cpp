#include<Aurora/ResourceTexture.h>

ResourceTexture::ResourceTexture(const std::string& filePath, const D3D11_USAGE& usage, const UINT& extraBindFlags, const UINT& cpuAccessFlag) :
	Texture2D(filePath, usage, D3D11_BIND_SHADER_RESOURCE | extraBindFlags, cpuAccessFlag)
{
	createShaderResource();
}

ResourceTexture::ResourceTexture(const unsigned int& width, const unsigned int& height, const TextureType& type) :
	Texture2D(width, height, type)
{
	createShaderResource();
}

ResourceTexture::ResourceTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const D3D11_USAGE& usage, const UINT& extraBindFlags, const bool& enableMSAA, const UINT& cpuAccessFlag) :
	Texture2D(width, height, format, usage, D3D11_BIND_SHADER_RESOURCE | extraBindFlags, enableMSAA, cpuAccessFlag)
{
	createShaderResource();
}

ResourceTexture::~ResourceTexture()
{
}

void ResourceTexture::createShaderResource()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
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

	Renderer::device->CreateShaderResourceView(texture.Get(), &srvDesc, shaderResourceView.ReleaseAndGetAddressOf());
}
