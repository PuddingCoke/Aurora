#include<Aurora/Core/DX/View/UnorderedAccessView.h>

UnorderedAccessView* UnorderedAccessView::curCUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

UnorderedAccessView* UnorderedAccessView::curPUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

ID3D11UnorderedAccessView* const UnorderedAccessView::nullUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

UnorderedAccessView::UnorderedAccessView() :
	CUAVSlot(-1), boundOnRTV(false)
{
}

UnorderedAccessView::~UnorderedAccessView()
{
	unbindFromCUAV(Renderer::getContext()) || unbindFromPUAV(Renderer::getContext());
}

ID3D11UnorderedAccessView* UnorderedAccessView::getUAV() const
{
	return unorderedAccessView.Get();
}

ID3D11UnorderedAccessView** UnorderedAccessView::releaseAndGetUAV()
{
	return unorderedAccessView.ReleaseAndGetAddressOf();
}

void UnorderedAccessView::unbindCUAV(ID3D11DeviceContext3* const ctx)
{
	for (unsigned int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++)
	{
		if (curCUAV[i])
		{
			curCUAV[i]->CUAVSlot = -1;
			curCUAV[i] = nullptr;
		}
	}
	ctx->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, nullUAV, nullptr);
}

void UnorderedAccessView::unbindPUAV(ID3D11DeviceContext3* const ctx)
{
	unsigned int num = 0;
	for (num = 0; curPUAV[num]; num++)
	{
		curPUAV[num]->boundOnRTV = false;
		curPUAV[num] = nullptr;
	}
	ctx->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 0, num, nullUAV, nullptr);
}

bool UnorderedAccessView::unbindFromCUAV(ID3D11DeviceContext3* const ctx)
{
	if (CUAVSlot != -1)
	{
		ctx->CSSetUnorderedAccessViews(CUAVSlot, 1, nullUAV, nullptr);
		curCUAV[CUAVSlot] = nullptr;
		CUAVSlot = -1;
		return true;
	}
	
	return false;
}

bool UnorderedAccessView::unbindFromPUAV(ID3D11DeviceContext3* const ctx)
{
	if (boundOnRTV)
	{
		unbindPUAV(ctx);
		return true;
	}

	return false;
}

void UnorderedAccessView::createUAV(ID3D11Resource* const resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc)
{
	Renderer::get()->createUnorderedAccessView(resource, &desc, unorderedAccessView.ReleaseAndGetAddressOf());
}
