#include<Aurora/DX/View/UnorderedAccessView.h>

UnorderedAccessView* UnorderedAccessView::curUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

ID3D11UnorderedAccessView* const UnorderedAccessView::nullUAV[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};

UnorderedAccessView::UnorderedAccessView() :
	UAVSlot(-1)
{
}

UnorderedAccessView::~UnorderedAccessView()
{
	unbindFromUAV();
}

ID3D11UnorderedAccessView* UnorderedAccessView::getUAV() const
{
	return unorderedAccessView.Get();
}

ID3D11UnorderedAccessView** UnorderedAccessView::releaseAndGetUAV()
{
	return unorderedAccessView.ReleaseAndGetAddressOf();
}

void UnorderedAccessView::unbindUAV()
{
	for (unsigned int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++)
	{
		if (curUAV[i])
		{
			curUAV[i]->UAVSlot = -1;
			curUAV[i] = nullptr;
		}
	}
	Renderer::getContext()->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, nullUAV, nullptr);
}

bool UnorderedAccessView::unbindFromUAV()
{
	if (UAVSlot != -1)
	{
		Renderer::getContext()->CSSetUnorderedAccessViews(UAVSlot, 1, nullUAV, nullptr);
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
