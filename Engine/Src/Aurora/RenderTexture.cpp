#include<Aurora/RenderTexture.h>

RenderTexture::~RenderTexture()
{
}

void RenderTexture::resolve(Texture2D* const texture2d) const
{
	RenderAPI::get()->ResolveSubresource(texture2d->get(), 0, texture.Get(), 0, format);
}

RenderTexture::RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4], const bool& enableMSAA) :
	Texture2D(width, height, format, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, enableMSAA)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;

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
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = format;

		if (enableMSAA)
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
		}

		createRTV(texture.Get(), rtvDesc);
	}
	
	clearRTV(color);
}
