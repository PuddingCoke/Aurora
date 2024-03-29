#include<Aurora/Core/DX/View/UnorderedAccessView.h>

ID3D11UnorderedAccessView* const UnorderedAccessView::nullUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

UnorderedAccessView::UnorderedAccessView() :
	CUAVSlot(-1), boundOnRTV(false)
{
}

UnorderedAccessView::UnorderedAccessView(const UnorderedAccessView& uav) :
	CUAVSlot(-1), boundOnRTV(false), unorderedAccessView(uav.unorderedAccessView)
{
}

UnorderedAccessView::~UnorderedAccessView()
{
}

ID3D11UnorderedAccessView* UnorderedAccessView::getUAV() const
{
	return unorderedAccessView.Get();
}

void UnorderedAccessView::unbindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	for (unsigned int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++)
	{
		if (states->curCUAV[i])
		{
			states->curCUAV[i]->CUAVSlot = -1;
			states->curCUAV[i] = nullptr;
		}
	}
	ctx->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, nullUAV, nullptr);
}

void UnorderedAccessView::unbindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	UINT num = 0;

	for (; num < D3D11_PS_CS_UAV_REGISTER_COUNT; num++)
	{
		if (states->curPUAV[num])
		{
			states->curPUAV[num]->boundOnRTV = false;
			states->curPUAV[num] = nullptr;
		}
		else
		{
			break;
		}
	}

	ctx->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 0, num, nullUAV, nullptr);
}

bool UnorderedAccessView::unbindFromCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	if (CUAVSlot != -1)
	{
		ctx->CSSetUnorderedAccessViews(CUAVSlot, 1, nullUAV, nullptr);
		states->curCUAV[CUAVSlot] = nullptr;
		CUAVSlot = -1;
		return true;
	}

	return false;
}

bool UnorderedAccessView::unbindFromPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	if (boundOnRTV)
	{
		unbindPUAV(ctx, states);
		return true;
	}

	return false;
}

void UnorderedAccessView::createUAV(ID3D11Resource* const resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc)
{
	GraphicsDevice::get()->createUnorderedAccessView(resource, &desc, unorderedAccessView.ReleaseAndGetAddressOf());
}
