#include<Aurora/Resource/DepthTexture.h>

DepthTexture::DepthTexture(const UINT& width, const UINT& height, const FMT& fmt, const bool& enableMSAA) :
	Texture2D(width, height, fmt, D3D11_BIND_DEPTH_STENCIL, enableMSAA)
{
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = FMTCAST(fmt);

		if (enableMSAA)
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}

		createDSV(texture.Get(), dsvDesc);
	}
}
