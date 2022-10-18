#include<Aurora/DX/View/UnorderedAccessView.h>

UnorderedAccessView* UnorderedAccessView::curUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

ID3D11UnorderedAccessView* const UnorderedAccessView::nullUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

UnorderedAccessView::UnorderedAccessView() :
	UAVSlot(-1)
{
}

UnorderedAccessView::~UnorderedAccessView()
{
}

ID3D11UnorderedAccessView* UnorderedAccessView::getUAV() const
{
	return unorderedAccessView.Get();
}

bool UnorderedAccessView::unbindFromUAV()
{
	if (UAVSlot != -1)
	{
		Renderer::context->CSSetUnorderedAccessViews(UAVSlot, 1, nullUAV, nullptr);
		curUAV[UAVSlot] = nullptr;
		UAVSlot = -1;
		return true;
	}

	return false;
}

void UnorderedAccessView::createUAV(ID3D11Resource* const resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc)
{
	Renderer::device->CreateUnorderedAccessView(resource, &desc, unorderedAccessView.ReleaseAndGetAddressOf());
}
