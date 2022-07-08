#include<Aurora/RenderTexture.h>

RenderTexture::~RenderTexture()
{
	delete texture;
}

RenderTexture* RenderTexture::create(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const DirectX::XMVECTORF32& color)
{
	return new RenderTexture(width, height, format, color);
}

void RenderTexture::setMSAARTV(const UINT& slot, ID3D11DepthStencilView* const view) const
{
	Graphics::context->OMSetRenderTargets(1, msaaTarget.GetAddressOf(), view);
}

void RenderTexture::setRTV(const UINT& slot, ID3D11DepthStencilView* const view) const
{
	Graphics::context->OMSetRenderTargets(1, normalTarget.GetAddressOf(), view);
}

void RenderTexture::resolve() const
{
	Graphics::context->ResolveSubresource(texture->texture2D.Get(), 0, msaaTexture.Get(), 0, format);
}

Texture2D* RenderTexture::getTexture() const
{
	return texture;
}

RenderTexture::RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4]) :
	width(width), height(height), format(format), texture(new Texture2D())
{
	//创建msaaTexture和msaaTarget
	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = width;
		tDesc.Height = height;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = format;
		tDesc.SampleDesc.Count = Graphics::msaaLevel;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		tDesc.CPUAccessFlags = 0;
		tDesc.MiscFlags = 0;

		Graphics::device->CreateTexture2D(&tDesc, nullptr, msaaTexture.ReleaseAndGetAddressOf());

		D3D11_RENDER_TARGET_VIEW_DESC msaaViewDesc = {};
		msaaViewDesc.Format = tDesc.Format;
		msaaViewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DMS;
		msaaViewDesc.Texture2D.MipSlice = 0;

		Graphics::device->CreateRenderTargetView(msaaTexture.Get(), &msaaViewDesc, msaaTarget.ReleaseAndGetAddressOf());
	}

	//创建texture
	{
		texture = Texture2D::create(width, height, format, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		D3D11_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = format;
		viewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		Graphics::device->CreateRenderTargetView(texture->texture2D.Get(), &viewDesc, normalTarget.ReleaseAndGetAddressOf());
	}

	Graphics::context->OMSetRenderTargets(1, msaaTarget.GetAddressOf(), nullptr);

	Graphics::context->ClearRenderTargetView(msaaTarget.Get(), color);

	resolve();
}
