#include<Aurora/Resource/ComputeTexture.h>

ComputeTexture::~ComputeTexture()
{
	delete[] mipArray;
}

ComputeTexture::ComputeTexture(const UINT& width, const UINT& height, const FMT& resFmt, const FMT& srvFmt, const FMT& uavFmt, const UINT& mipLevels, const UINT& arraySize) :
	Texture2D(width, height, mipLevels, arraySize, resFmt, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)
{
	//global srv
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(srvFmt);

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
		mipArray = new USViewEx[mipLevels];

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(srvFmt);
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = FMTCAST(uavFmt);

		if (arraySize > 1)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;

			for (UINT i = 0; i < mipLevels; i++)
			{
				srvDesc.Texture2DArray.ArraySize = arraySize;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.MipLevels = 1;
				srvDesc.Texture2DArray.MostDetailedMip = i;

				uavDesc.Texture2DArray.ArraySize = arraySize;
				uavDesc.Texture2DArray.FirstArraySlice = 0;
				uavDesc.Texture2DArray.MipSlice = i;

				mipArray[i].createSRV(texture.Get(), srvDesc);
				mipArray[i].createUAV(texture.Get(), uavDesc);
				mipArray[i].allSRV = this;
			}
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

			for (UINT i = 0; i < mipLevels; i++)
			{
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.MostDetailedMip = i;

				uavDesc.Texture2D.MipSlice = i;

				mipArray[i].createSRV(texture.Get(), srvDesc);
				mipArray[i].createUAV(texture.Get(), uavDesc);
				mipArray[i].allSRV = this;
			}
		}
	}
}

USView* ComputeTexture::getMip(const UINT& index)
{
	return &mipArray[index];
}

void ComputeTexture::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		mipArray[i].unbindFromCUAV(ctx, states) || mipArray[i].unbindFromPUAV(ctx, states);
	}
}

void ComputeTexture::USViewEx::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromCUAV(ctx,states) || unbindFromPUAV(ctx,states);
}

void ComputeTexture::USViewEx::bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	allSRV->unbindFromSRV(ctx,states);

	unbindFromPUAV(ctx, states) || unbindFromSRV(ctx, states);
}

void ComputeTexture::USViewEx::bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	allSRV->unbindFromSRV(ctx, states);

	unbindFromCUAV(ctx, states) || unbindFromSRV(ctx, states);
}
