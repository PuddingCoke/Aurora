#include<Aurora/Core/DX/View/RenderTargetView.h>

RenderTargetView::RenderTargetView() :
	boundOnRTV(false)
{
}

RenderTargetView::RenderTargetView(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc) :
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

void RenderTargetView::unbindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	for (unsigned int i = 0; states->curRTV[i]; i++)
	{
		states->curRTV[i]->boundOnRTV = false;
		states->curRTV[i] = nullptr;
	}
	ctx->OMSetRenderTargets(0, nullptr, nullptr);
}

bool RenderTargetView::unbindFromRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	if (boundOnRTV)
	{
		unbindRTV(ctx, states);
		return true;
	}

	return false;
}

void RenderTargetView::createRTV(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc)
{
	GraphicsDevice::get()->createRenderTargetView(resource, &desc, renderTargetView.ReleaseAndGetAddressOf());
}
