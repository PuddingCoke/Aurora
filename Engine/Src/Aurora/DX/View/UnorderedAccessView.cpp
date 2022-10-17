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

ID3D11UnorderedAccessView** UnorderedAccessView::ReleaseAndGetAddressOf()
{
	return unorderedAccessView.ReleaseAndGetAddressOf();
}

bool UnorderedAccessView::unbindFromUAV()
{
	bool success = false;

	if (UAVSlot != -1)
	{
		Renderer::context->CSSetUnorderedAccessViews(UAVSlot, 1, nullUAV, nullptr);
		curUAV[UAVSlot] = nullptr;
		UAVSlot = -1;
		success = true;
	}

	return success;
}
