#include<Aurora/DX/View/RenderTargetView.h>

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
	unbindFromRTV();
}

void RenderTargetView::clearRTV(const float* color)
{
	Renderer::getContext()->ClearRenderTargetView(renderTargetView.Get(), color);
}

ID3D11RenderTargetView* RenderTargetView::getRTV() const
{
	return renderTargetView.Get();
}

ID3D11RenderTargetView** RenderTargetView::releaseAndGetRTV()
{
	return renderTargetView.ReleaseAndGetAddressOf();
}

void RenderTargetView::unbindRTV()
{
	for (unsigned int i = 0; curRTV[i]; i++)
	{
		curRTV[i]->boundOnRTV = false;
		curRTV[i] = nullptr;
	}
	Renderer::getContext()->OMSetRenderTargets(0, nullptr, nullptr);
}

bool RenderTargetView::unbindFromRTV()
{
	if (boundOnRTV)
	{
		unbindRTV();
		return true;
	}

	return false;
}

void RenderTargetView::createRTV(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc)
{
	Renderer::device->CreateRenderTargetView(resource, &desc, renderTargetView.ReleaseAndGetAddressOf());
}
