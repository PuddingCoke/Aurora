#include<Aurora/Core/DX/View/RenderTargetView.h>

RenderTargetView* RenderTargetView::curRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

RenderTargetView::RenderTargetView() :
	boundOnRTV(false)
{
}

RenderTargetView::RenderTargetView(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc):
	boundOnRTV(false)
{
	createRTV(resource, desc);
}

RenderTargetView::~RenderTargetView()
{
}

ID3D11RenderTargetView* RenderTargetView::getRTV() const
{
	return renderTargetView.Get();
}

ID3D11RenderTargetView** RenderTargetView::releaseAndGetRTV()
{
	return renderTargetView.ReleaseAndGetAddressOf();
}

void RenderTargetView::unbindRTV(ID3D11DeviceContext3* const ctx)
{
	for (unsigned int i = 0; curRTV[i]; i++)
	{
		curRTV[i]->boundOnRTV = false;
		curRTV[i] = nullptr;
	}
	ctx->OMSetRenderTargets(0, nullptr, nullptr);
}

bool RenderTargetView::unbindFromRTV(ID3D11DeviceContext3* const ctx)
{
	if (boundOnRTV)
	{
		unbindRTV(ctx);
		return true;
	}

	return false;
}

void RenderTargetView::createRTV(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc)
{
	Renderer::get()->createRenderTargetView(resource, &desc, renderTargetView.ReleaseAndGetAddressOf());
}
