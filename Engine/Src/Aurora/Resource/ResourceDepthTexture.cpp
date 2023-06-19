#include<Aurora/Resource/ResourceDepthTexture.h>

ResourceDepthTexture::ResourceDepthTexture(const UINT& width, const UINT& height, const FMT& resFmt, const FMT& srvFmt, const FMT& dsvFmt) :
	Texture2D(width, height, resFmt, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL, false)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(srvFmt);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		createSRV(texture.Get(), srvDesc);
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = FMTCAST(dsvFmt);
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		createDSV(texture.Get(), dsvDesc);
	}
}

void ResourceDepthTexture::bindDSV(ID3D11DeviceContext3* const ctx)
{
	unbindFromSRV(ctx);
}

void ResourceDepthTexture::bindSRV(ID3D11DeviceContext3* const ctx)
{

}
