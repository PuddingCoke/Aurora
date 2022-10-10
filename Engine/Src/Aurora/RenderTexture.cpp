#include<Aurora/RenderTexture.h>

ID3D11RenderTargetView* RenderTexture::renderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

RenderTexture::~RenderTexture()
{
}

void RenderTexture::setMSAARTV(ID3D11DepthStencilView* const view) const
{
	Renderer::context->OMSetRenderTargets(1, msaaTarget.GetAddressOf(), view);
}

ID3D11RenderTargetView* RenderTexture::getRTV() const
{
	return normalTarget.Get();
}

void RenderTexture::setRTV(ID3D11DepthStencilView* const view) const
{
	Renderer::context->OMSetRenderTargets(1, normalTarget.GetAddressOf(), view);
}

void RenderTexture::clearMSAARTV(const float color[4]) const
{
	Renderer::context->ClearRenderTargetView(msaaTarget.Get(), color);
}

void RenderTexture::clearRTV(const float color[4]) const
{
	Renderer::context->ClearRenderTargetView(normalTarget.Get(), color);
}

void RenderTexture::resolve() const
{
	Renderer::context->ResolveSubresource(texture.Get(), 0, msaaTexture.Get(), 0, format);
}

void RenderTexture::setRTVs(std::initializer_list<RenderTexture*> renderTextures, ID3D11DepthStencilView* view)
{
	std::initializer_list<RenderTexture*>::iterator it = renderTextures.begin();
	for (int i = 0; i < renderTextures.size(); i++)
	{
		renderTargetViews[i] = it[0]->normalTarget.Get();
		++it;
	}
	Renderer::context->OMSetRenderTargets((UINT)renderTextures.size(), renderTargetViews, view);
}

void RenderTexture::setMSAARTVs(std::initializer_list<RenderTexture*> renderTextures, ID3D11DepthStencilView* msaaView)
{
	std::initializer_list<RenderTexture*>::iterator it = renderTextures.begin();
	for (int i = 0; i < renderTextures.size(); i++)
	{
		renderTargetViews[i] = it[0]->msaaTarget.Get();
		++it;
	}
	Renderer::context->OMSetRenderTargets((UINT)renderTextures.size(), renderTargetViews, msaaView);
}

void RenderTexture::unbindAll()
{
	renderTargetViews[0] = nullptr;
	renderTargetViews[1] = nullptr;
	renderTargetViews[2] = nullptr;
	renderTargetViews[3] = nullptr;
	renderTargetViews[4] = nullptr;
	renderTargetViews[5] = nullptr;
	renderTargetViews[6] = nullptr;
	renderTargetViews[7] = nullptr;
	Renderer::context->OMSetRenderTargets(8, renderTargetViews, nullptr);
}

RenderTexture::RenderTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4], const bool& enableMSAA) :
	Texture2D(width, height, format, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
{
	//创建普通材质的RenderTargetView
	{
		D3D11_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = format;
		viewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateRenderTargetView(texture.Get(), &viewDesc, normalTarget.ReleaseAndGetAddressOf());
	}

	if (enableMSAA)//创建抗锯齿材质以及RenderTargetView
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

		Renderer::device->CreateTexture2D(&tDesc, nullptr, msaaTexture.ReleaseAndGetAddressOf());

		D3D11_RENDER_TARGET_VIEW_DESC msaaViewDesc = {};
		msaaViewDesc.Format = tDesc.Format;
		msaaViewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DMS;
		msaaViewDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateRenderTargetView(msaaTexture.Get(), &msaaViewDesc, msaaTarget.ReleaseAndGetAddressOf());

		Renderer::context->ClearRenderTargetView(msaaTarget.Get(), color);

		resolve();
	}
	else
	{
		Renderer::context->ClearRenderTargetView(normalTarget.Get(), color);
	}
}
