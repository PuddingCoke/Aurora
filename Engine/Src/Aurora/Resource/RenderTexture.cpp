#include<Aurora/Resource/RenderTexture.h>

RenderTexture::~RenderTexture()
{
	delete[] mipArray;
}

RSView* RenderTexture::getMip(const UINT& index)
{
	return &mipArray[index];
}

void RenderTexture::bindSRV()
{
	for (UINT i = 0; i < mipLevels; i++)
	{
		if (mipArray[i].unbindFromRTV())
		{
			break;
		}
	}
}

void RenderTexture::clearRTV(const float color[4], const UINT& index) const
{
	mipArray[index].clearRTV(color);
}

RenderTexture::RenderTexture(const UINT& width, const UINT& height, const FMT& fmt, const float color[4], const bool& enableMSAA) :
	Texture2D(width, height, fmt, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, enableMSAA)
{
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

	{
		mipArray = new RSViewEx[1];

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = FMTCAST(fmt);

		if (enableMSAA)
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
		}

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

		mipArray[0].createRTV(texture.Get(), rtvDesc);
		mipArray[0].createSRV(texture.Get(), srvDesc);
		mipArray[0].allSRV = this;
	}

	mipArray[0].clearRTV(color);
}

RenderTexture::RenderTexture(const UINT& width, const UINT& height, const FMT& fmt, const UINT& mipLevels, const UINT& arraySize, const float color[4]) :
	Texture2D(width, height, mipLevels, arraySize, fmt, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 0)
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
		mipArray = new RSViewEx[mipLevels];

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(fmt);
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = FMTCAST(fmt);

		if (arraySize > 1)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;

			for (UINT i = 0; i < mipLevels; i++)
			{
				srvDesc.Texture2DArray.ArraySize = arraySize;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.MipLevels = 1;
				srvDesc.Texture2DArray.MostDetailedMip = i;

				rtvDesc.Texture2DArray.ArraySize = arraySize;
				rtvDesc.Texture2DArray.FirstArraySlice = 0;
				rtvDesc.Texture2DArray.MipSlice = i;

				mipArray[i].createSRV(texture.Get(), srvDesc);
				mipArray[i].createRTV(texture.Get(), rtvDesc);
				mipArray[i].allSRV = this;
			}
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			for (UINT i = 0; i < mipLevels; i++)
			{
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.MostDetailedMip = i;

				rtvDesc.Texture2D.MipSlice = i;

				mipArray[i].createSRV(texture.Get(), srvDesc);
				mipArray[i].createRTV(texture.Get(), rtvDesc);
				mipArray[i].allSRV = this;
			}
		}
	}

	for (UINT i = 0; i < mipLevels; i++)
	{
		mipArray[i].clearRTV(color);
	}
}

void RenderTexture::RSViewEx::bindRTV()
{
	allSRV->unbindFromSRV();

	unbindFromSRV();
}

void RenderTexture::RSViewEx::bindSRV()
{
	unbindFromRTV();
}
