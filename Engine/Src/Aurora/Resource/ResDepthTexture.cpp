#include<Aurora/Resource/ResDepthTexture.h>

ResDepthTexture::ResDepthTexture(const UINT& width, const UINT& height, const DXGI_FORMAT& texFormat, const DXGI_FORMAT& srvFormat, const DXGI_FORMAT& dsvFormat) :
	Texture2D(width, height, texFormat, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL, false)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = srvFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		createSRV(texture.Get(), srvDesc);
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dsvFormat;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		createDSV(texture.Get(), dsvDesc);
	}
}

void ResDepthTexture::bindDSV()
{
	unbindFromSRV();
}

void ResDepthTexture::bindSRV()
{

}
