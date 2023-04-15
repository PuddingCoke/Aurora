#include<Aurora/CTextureWithMips.h>

CTextureWithMips::CTextureWithMips(const UINT& width, const UINT& height, const DXGI_FORMAT& format, const UINT& mipLevels, const UINT& arraySize) :
	Texture2D(width, height, mipLevels, arraySize, format, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0),
	uav(new CustomedUAV[mipLevels])
{
	if (arraySize > 1)
	{
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize = arraySize;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.MipLevels = mipLevels;
			srvDesc.Texture2DArray.MostDetailedMip = 0;

			createSRV(texture.Get(), srvDesc);
		}

		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = format;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.Texture2DArray.ArraySize = arraySize;
			uavDesc.Texture2DArray.FirstArraySlice = 0;

			for (UINT i = 0; i < mipLevels; i++)
			{
				uavDesc.Texture2DArray.MipSlice = i;

				uav[i].createUAV(texture.Get(), uavDesc);
			}
		}
	}
	else
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

			for (UINT i = 0; i < mipLevels; i++)
			{
				uavDesc.Texture2D.MipSlice = i;

				uav[i].createUAV(texture.Get(), uavDesc);
			}
		}
	}
}

CTextureWithMips::~CTextureWithMips()
{
	delete[] uav;
}

UnorderedAccessView* CTextureWithMips::getUAV(const UINT& mipSlice)
{
	unbindFromSRV();

	return &uav[mipSlice];
}

void CTextureWithMips::bindSRV()
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		uav[i].unbindFromCUAV() || uav[i].unbindFromPUAV();
	}
}

CTextureWithMips::CustomedUAV::CustomedUAV()
{
}

void CTextureWithMips::CustomedUAV::bindCUAV()
{
	unbindFromPUAV();
}

void CTextureWithMips::CustomedUAV::bindPUAV()
{
	unbindFromCUAV();
}
