#include<Aurora/RCTexture.h>

RCTexture::RCTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4]) :
	ComputeTexture(width, height, format, D3D11_BIND_RENDER_TARGET)
{
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	Renderer::device->CreateRenderTargetView(texture.Get(), &rtvDesc, textureRTV.ReleaseAndGetAddressOf());

	Renderer::context->ClearRenderTargetView(textureRTV.Get(), color);
}

RCTexture::~RCTexture()
{
}

void RCTexture::clearRTV(const float color[4]) const
{
	Renderer::context->ClearRenderTargetView(textureRTV.Get(), color);
}

void RCTexture::setRTV(ID3D11DepthStencilView* const view) const
{
	Renderer::context->OMSetRenderTargets(1, textureRTV.GetAddressOf(), view);
}
