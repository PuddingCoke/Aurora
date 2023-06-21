#include<Aurora/Resource/RenderComputeTexture.h>

RenderComputeTexture::~RenderComputeTexture()
{
	delete[] mipArray;
}

RUSView* RenderComputeTexture::getMip(const UINT& index)
{
	return &mipArray[index];
}

void RenderComputeTexture::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		if (mipArray[i].unbindFromRTV(ctx, states))
		{
			break;
		}
	}

	for (UINT i = 0; i < mipLevels; i++)
	{
		mipArray[i].unbindFromCUAV(ctx, states) || mipArray[i].unbindFromPUAV(ctx, states);
	}
}

RenderComputeTexture::RenderComputeTexture(const UINT& width, const UINT& height, const FMT& fmt, const float color[4], const UINT& mipLevels, const UINT& arraySize) :
	Texture2D(width, height, mipLevels, arraySize, fmt, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET, 0)
{
	//global srv
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(fmt);

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
		mipArray = new RUSViewEx[mipLevels];

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(fmt);
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = FMTCAST(fmt);
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = FMTCAST(fmt);

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
				mipArray[i].allSRV = this;
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
				mipArray[i].allSRV = this;
			}
		}
	}
}

RenderComputeTexture::RenderComputeTexture(const RenderComputeTexture& t)
{
	width = t.width;
	height = t.height;
	arraySize = t.arraySize;
	mipLevels = t.mipLevels;
	format = t.format;
	texture = t.texture;

	shaderResourceView = t.shaderResourceView;

	mipArray = new RUSViewEx[t.mipLevels];

	for (UINT i = 0; i < mipLevels; i++)
	{
		mipArray[i] = t.mipArray[i];
	}
}

void RenderComputeTexture::RUSViewEx::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromCUAV(ctx, states) || unbindFromPUAV(ctx, states) || unbindFromRTV(ctx, states);
}

void RenderComputeTexture::RUSViewEx::bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	allSRV->unbindFromSRV(ctx, states);

	unbindFromSRV(ctx, states) || unbindFromPUAV(ctx, states) || unbindFromRTV(ctx, states);
}

void RenderComputeTexture::RUSViewEx::bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	allSRV->unbindFromSRV(ctx, states);

	unbindFromSRV(ctx, states) || unbindFromCUAV(ctx, states) || unbindFromRTV(ctx, states);
}

void RenderComputeTexture::RUSViewEx::bindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	allSRV->unbindFromSRV(ctx, states);

	unbindFromSRV(ctx, states) || unbindFromCUAV(ctx, states) || unbindFromPUAV(ctx, states);
}
