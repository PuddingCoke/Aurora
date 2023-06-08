#include<Aurora/Resource/RenderComputeTexture.h>

RenderComputeTexture::~RenderComputeTexture()
{
	delete[] mipArray;
}

RenderTargetView* RenderComputeTexture::getRTVMip(const UINT& index)
{
	unbindFromSRV();

	return &mipArray[index];
}

UnorderedAccessView* RenderComputeTexture::getUAVMip(const UINT& index)
{
	unbindFromSRV();

	return &mipArray[index];
}

ShaderResourceView* RenderComputeTexture::getSRVMip(const UINT& index)
{
	return &mipArray[index];
}

void RenderComputeTexture::bindSRV()
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		if (mipArray[i].unbindFromRTV())
		{
			break;
		}
	}

	for (UINT i = 0; i < mipLevels; i++)
	{
		mipArray[i].unbindFromCUAV() || mipArray[i].unbindFromPUAV();
	}
}

void RenderComputeTexture::clearUAV(const float* const color, const UINT& index) const
{
	mipArray[index].clearUAV(color);
}

void RenderComputeTexture::clearUAV(const unsigned int* const value, const UINT& index) const
{
	mipArray[index].clearUAV(value);
}

void RenderComputeTexture::clearRTV(const float color[4], const UINT& index) const
{
	mipArray[index].clearRTV(color);
}

RenderComputeTexture::RenderComputeTexture(const UINT& width, const UINT& height, const DXGI_FORMAT& format, const float color[4], const UINT& mipLevels, const UINT& arraySize) :
	Texture2D(width, height, format, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET)
{
	//global srv
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;

		if (arraySize > 1)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize = arraySize;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.MipLevels = mipLevels;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = mipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
		}

		createSRV(texture.Get(), srvDesc);
	}

	{
		mipArray = new RUSView[mipLevels];

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = format;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = format;

		if (arraySize > 1)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;

			for (UINT i = 0; i < mipLevels; i++)
			{
				srvDesc.Texture2DArray.ArraySize = arraySize;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.MipLevels = 1;
				srvDesc.Texture2DArray.MostDetailedMip = i;

				rtvDesc.Texture2DArray.ArraySize = arraySize;
				rtvDesc.Texture2DArray.FirstArraySlice = 0;
				rtvDesc.Texture2DArray.MipSlice = i;

				uavDesc.Texture2DArray.ArraySize = arraySize;
				uavDesc.Texture2DArray.FirstArraySlice = 0;
				uavDesc.Texture2DArray.MipSlice = i;

				mipArray[i].createSRV(texture.Get(), srvDesc);
				mipArray[i].createRTV(texture.Get(), rtvDesc);
				mipArray[i].createUAV(texture.Get(), uavDesc);
			}
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

			for (UINT i = 0; i < mipLevels; i++)
			{
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.MostDetailedMip = i;

				rtvDesc.Texture2D.MipSlice = i;

				uavDesc.Texture2D.MipSlice = i;

				mipArray[i].createSRV(texture.Get(), srvDesc);
				mipArray[i].createRTV(texture.Get(), rtvDesc);
				mipArray[i].createUAV(texture.Get(), uavDesc);
			}
		}
	}
}
