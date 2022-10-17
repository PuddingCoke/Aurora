#include<Aurora/DX/View/RenderTargetView.h>

RenderTargetView* RenderTargetView::curRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

ID3D11RenderTargetView* const RenderTargetView::nullRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

RenderTargetView::RenderTargetView() :
	boundOnRTV(false)
{
}

RenderTargetView::~RenderTargetView()
{
}

void RenderTargetView::clearRTV(const float* color)
{
	Renderer::context->ClearRenderTargetView(renderTargetView.Get(), color);
}

void RenderTargetView::unbindRTV()
{
	for (unsigned int i = 0; curRTV[i]; i++)
	{
		curRTV[i]->boundOnRTV = false;
		curRTV[i] = nullptr;
	}
	Renderer::context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullRTV, nullptr);
}

bool RenderTargetView::unbindFromRTV()
{
	bool success = false;

	if (boundOnRTV)
	{
		unbindRTV();
		success = true;
	}

	return success;
}
