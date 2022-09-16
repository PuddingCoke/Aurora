#include<Aurora/RWTexture.h>

RWTexture* RWTexture::create(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4])
{
	return new RWTexture(width, height, format, color);
}

RWTexture::~RWTexture()
{
	delete texture;
}

void RWTexture::clearRTV(const float color[4]) const
{
	Renderer::context->ClearRenderTargetView(textureRTV.Get(), color);
}

void RWTexture::setRTV(ID3D11DepthStencilView* const view) const
{
	Renderer::context->OMSetRenderTargets(1, textureRTV.GetAddressOf(), view);
}

void RWTexture::PSSetSRV(const UINT& slot) const
{
	texture->setSRV(slot);
}

void RWTexture::CSSetSRV(const UINT& slot) const
{
	ID3D11ShaderResourceView* const textureSRV = texture->getSRV();
	Renderer::context->CSSetShaderResources(slot, 1, &textureSRV);
}

void RWTexture::setUAV(const UINT& slot) const
{
	Renderer::context->CSSetUnorderedAccessViews(slot, 1, textureUAV.GetAddressOf(), nullptr);
}

RWTexture::RWTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4]) :
	width(width), height(height), format(format), texture(Texture2D::create(width, height, format, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0))
{
	//创建RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateRenderTargetView(texture->getTexture2D(), &rtvDesc, textureRTV.ReleaseAndGetAddressOf());
	}

	//创建UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateUnorderedAccessView(texture->getTexture2D(), &uavDesc, textureUAV.ReleaseAndGetAddressOf());
	}

	Renderer::context->ClearRenderTargetView(textureRTV.Get(), color);
}
