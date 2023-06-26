#include<Aurora/Core/DX/View/RenderTargetView.h>

ID3D11RenderTargetView* const RenderTargetView::nullRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

RenderTargetView::RenderTargetView() :
	boundOnRTV(false)
{
}

RenderTargetView::RenderTargetView(const RenderTargetView& rtv) :
	boundOnRTV(false), renderTargetView(rtv.renderTargetView)
{
}

RenderTargetView::~RenderTargetView()
{
}

ID3D11RenderTargetView* RenderTargetView::getRTV() const
{
	return renderTargetView.Get();
}

void RenderTargetView::unbindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	UINT num = 0;

	for (; num < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; num++)
	{
		if (states->curRTV[num])
		{
			states->curRTV[num]->boundOnRTV = false;
			states->curRTV[num] = nullptr;
		}
		else
		{
			break;
		}
	}

	ctx->OMSetRenderTargets(num, nullRTV, nullptr);
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
