#include<Aurora/Resource/ComputeTexture3D.h>

ComputeTexture3D::ComputeTexture3D(const UINT& width, const UINT& height, const UINT& depth, const FMT& fmt, const UINT& extraBindFlags, const UINT& miscFlags, const UINT& mipLevels) :
	Texture3D(width, height, depth, fmt, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | extraBindFlags, miscFlags, mipLevels)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(fmt);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels = mipLevels;
		srvDesc.Texture3D.MostDetailedMip = 0;

		createSRV(texture.Get(), srvDesc);
	}

	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = FMTCAST(fmt);
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.FirstWSlice = 0;
		uavDesc.Texture3D.MipSlice = 0;
		uavDesc.Texture3D.WSize = depth;

		createUAV(texture.Get(), uavDesc);
	}
}

ComputeTexture3D::ComputeTexture3D(const UINT& width, const UINT& height, const UINT& depth, const FMT& resFmt, const FMT& srvFmt, const FMT& uavFmt, const UINT& extraBindFlags, const UINT& miscFlags, const UINT& mipLevels):
	Texture3D(width, height, depth, resFmt, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | extraBindFlags, miscFlags, mipLevels)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(srvFmt);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels = mipLevels;
		srvDesc.Texture3D.MostDetailedMip = 0;

		createSRV(texture.Get(), srvDesc);
	}

	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = FMTCAST(uavFmt);
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.FirstWSlice = 0;
		uavDesc.Texture3D.MipSlice = 0;
		uavDesc.Texture3D.WSize = depth;

		createUAV(texture.Get(), uavDesc);
	}
}

ComputeTexture3D::~ComputeTexture3D()
{
}