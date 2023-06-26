#include<Aurora/Resource/ComputeTexture3D.h>

ComputeTexture3D::ComputeTexture3D(const UINT& width, const UINT& height, const UINT& depth, const FMT& resFmt, const FMT& srvFmt, const FMT& uavFmt, const UINT& extraBindFlags, const UINT& miscFlags, const UINT& mipLevels) :
	Texture3D(width, height, depth, resFmt, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | extraBindFlags, miscFlags, mipLevels)
{
	//global srv
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(srvFmt);

		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels = mipLevels;
		srvDesc.Texture3D.MostDetailedMip = 0;

		createSRV(texture.Get(), srvDesc);
	}

	{
		mipArray = new USViewEx[mipLevels];

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(srvFmt);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = FMTCAST(uavFmt);
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

		for (UINT i = 0; i < mipLevels; i++)
		{
			srvDesc.Texture3D.MipLevels = 1;
			srvDesc.Texture3D.MostDetailedMip = i;

			uavDesc.Texture3D.FirstWSlice = 0;
			uavDesc.Texture3D.MipSlice = i;
			uavDesc.Texture3D.WSize = -1;

			mipArray[i].createSRV(texture.Get(), srvDesc);
			mipArray[i].createUAV(texture.Get(), uavDesc);
			mipArray[i].allSRV = this;
		}
	}
}

ComputeTexture3D::ComputeTexture3D(const ComputeTexture3D& t) :
	Texture3D(t), ShaderResourceView(t)
{
	mipArray = new USViewEx[t.mipLevels];

	for (UINT i = 0; i < mipLevels; i++)
	{
		mipArray[i] = t.mipArray[i];
	}
}

ComputeTexture3D::~ComputeTexture3D()
{
	delete[] mipArray;
}

USView* ComputeTexture3D::getMip(const UINT& index)
{
	return &mipArray[index];
}

void ComputeTexture3D::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		mipArray[i].unbindFromCUAV(ctx, states) || mipArray[i].unbindFromPUAV(ctx, states);
	}
}

void ComputeTexture3D::USViewEx::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromCUAV(ctx, states) || unbindFromPUAV(ctx, states);
}

void ComputeTexture3D::USViewEx::bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	allSRV->unbindFromSRV(ctx, states);

	unbindFromPUAV(ctx, states) || unbindFromSRV(ctx, states);
}

void ComputeTexture3D::USViewEx::bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	allSRV->unbindFromSRV(ctx, states);

	unbindFromCUAV(ctx, states) || unbindFromSRV(ctx, states);
}