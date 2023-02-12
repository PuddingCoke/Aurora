#include<Aurora/ComputeTexture.h>

ComputeTexture::ComputeTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format) :
	Texture2D(width, height, format, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = mipLevels;

		createSRV(texture.Get(), srvDesc);
	}

	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		createUAV(texture.Get(), uavDesc);
	}
}

ComputeTexture::~ComputeTexture()
{
}
