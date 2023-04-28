#include<Aurora/DX/View/UnorderedAccessView.h>

UnorderedAccessView* UnorderedAccessView::curCUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

UnorderedAccessView* UnorderedAccessView::curPUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

ID3D11UnorderedAccessView* const UnorderedAccessView::nullUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

UnorderedAccessView::UnorderedAccessView() :
	CUAVSlot(-1), boundOnRTV(false)
{
}

UnorderedAccessView::~UnorderedAccessView()
{
	unbindFromCUAV() || unbindFromPUAV();
}

ID3D11UnorderedAccessView* UnorderedAccessView::getUAV() const
{
	return unorderedAccessView.Get();
}

ID3D11UnorderedAccessView** UnorderedAccessView::releaseAndGetUAV()
{
	return unorderedAccessView.ReleaseAndGetAddressOf();
}

void UnorderedAccessView::unbindCUAV()
{
	for (unsigned int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++)
	{
		if (curCUAV[i])
		{
			curCUAV[i]->CUAVSlot = -1;
			curCUAV[i] = nullptr;
		}
	}
	Renderer::getContext()->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, nullUAV, nullptr);
}

void UnorderedAccessView::unbindPUAV()
{
	unsigned int num = 0;
	for (num = 0; curPUAV[num]; num++)
	{
		curPUAV[num]->boundOnRTV = false;
		curPUAV[num] = nullptr;
	}
	Renderer::getContext()->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 0, num, nullUAV, nullptr);
}

void UnorderedAccessView::clear(const float* const color) const
{
	Renderer::getContext()->ClearUnorderedAccessViewFloat(unorderedAccessView.Get(), color);
}

void UnorderedAccessView::clear(const unsigned int* const value) const
{
	Renderer::getContext()->ClearUnorderedAccessViewUint(unorderedAccessView.Get(), value);
}

bool UnorderedAccessView::unbindFromCUAV()
{
	if (CUAVSlot != -1)
	{
		Renderer::getContext()->CSSetUnorderedAccessViews(CUAVSlot, 1, nullUAV, nullptr);
		curCUAV[CUAVSlot] = nullptr;
		CUAVSlot = -1;
		return true;
	}
	
	return false;
}

bool UnorderedAccessView::unbindFromPUAV()
{
	if (boundOnRTV)
	{
		unbindPUAV();
		return true;
	}

	return false;
}

void UnorderedAccessView::createUAV(ID3D11Resource* const resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc)
{
	Renderer::getDevice()->CreateUnorderedAccessView(resource, &desc, unorderedAccessView.ReleaseAndGetAddressOf());
}
