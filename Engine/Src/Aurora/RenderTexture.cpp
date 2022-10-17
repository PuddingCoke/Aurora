#include<Aurora/RenderTexture.h>

RenderTexture::~RenderTexture()
{
}

ID3D11RenderTargetView* RenderTexture::getRTV() const
{
	return renderTargetView.Get();
}

void RenderTexture::resolve(Texture2D* const texture) const
{
	Renderer::context->ResolveSubresource(texture->getTexture2D(), 0, this->texture.Get(), 0, format);
}

RenderTexture::RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4], const bool& enableMSAA) :
	Texture2D(width, height, format, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, enableMSAA)
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

		Renderer::device->CreateShaderResourceView(texture.Get(), &srvDesc, shaderResourceView.ReleaseAndGetAddressOf());
	}

	{
		D3D11_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = format;

		if (enableMSAA)
		{
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			viewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
			viewDesc.Texture2D.MipSlice = 0;
		}

		Renderer::device->CreateRenderTargetView(texture.Get(), &viewDesc, renderTargetView.ReleaseAndGetAddressOf());
	}

	Renderer::context->ClearRenderTargetView(renderTargetView.Get(), color);
}
