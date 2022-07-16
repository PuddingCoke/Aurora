#include<Aurora/RenderTexture.h>

ID3D11RenderTargetView* RenderTexture::renderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

RenderTexture::~RenderTexture()
{
	delete texture;
}

RenderTexture* RenderTexture::create(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4],const bool& enableMSAA)
{
	return new RenderTexture(width, height, format, color, enableMSAA);
}

void RenderTexture::setMSAARTV(ID3D11DepthStencilView* const view) const
{
	Graphics::context->OMSetRenderTargets(1, msaaTarget.GetAddressOf(), view);
}

void RenderTexture::setRTV(ID3D11DepthStencilView* const view) const
{
	Graphics::context->OMSetRenderTargets(1, normalTarget.GetAddressOf(), view);
}

void RenderTexture::clearMSAARTV(const float color[4])
{
	Graphics::context->ClearRenderTargetView(msaaTarget.Get(), color);
}

void RenderTexture::clearRTV(const float color[4])
{
	Graphics::context->ClearRenderTargetView(normalTarget.Get(), color);
}

void RenderTexture::resolve() const
{
	Graphics::context->ResolveSubresource(texture->getTexture2D(), 0, msaaTexture.Get(), 0, format);
}

Texture2D* RenderTexture::getTexture() const
{
	return texture;
}

void RenderTexture::setRTVs(std::initializer_list<RenderTexture*> renderTextures, ID3D11DepthStencilView* view)
{
	std::initializer_list<RenderTexture*>::iterator it = renderTextures.begin();
	for (int i = 0; i < renderTextures.size(); i++)
	{
		renderTargetViews[i] = it[0]->normalTarget.Get();
		++it;
	}
	Graphics::context->OMSetRenderTargets((UINT)renderTextures.size(), renderTargetViews, view);
}

void RenderTexture::setMSAARTVs(std::initializer_list<RenderTexture*> renderTextures, ID3D11DepthStencilView* msaaView)
{
	std::initializer_list<RenderTexture*>::iterator it = renderTextures.begin();
	for (int i = 0; i < renderTextures.size(); i++)
	{
		renderTargetViews[i] = it[0]->msaaTarget.Get();
		++it;
	}
	Graphics::context->OMSetRenderTargets((UINT)renderTextures.size(), renderTargetViews, msaaView);
}

RenderTexture::RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4],const bool& enableMSAA) :
	width(width), height(height), format(format), texture(Texture2D::create(width, height, format, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE))
{
	//创建普通材质的RenderTargetView
	{
		D3D11_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = format;
		viewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		Graphics::device->CreateRenderTargetView(texture->getTexture2D(), &viewDesc, normalTarget.ReleaseAndGetAddressOf());
	}

	if(enableMSAA)//创建抗锯齿材质以及RenderTargetView
	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = width;
		tDesc.Height = height;
		tDesc.MipLevels = 1;
		tDesc.ArraySize = 1;
		tDesc.Format = format;
		tDesc.SampleDesc.Count = Graphics::getMSAALevel();
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

		Graphics::context->ClearRenderTargetView(msaaTarget.Get(), color);

		resolve();
	}
	else
	{
		Graphics::context->ClearRenderTargetView(normalTarget.Get(), color);
	}
}
