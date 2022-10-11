#include<Aurora/DX/UnorderedAccessView.h>

UnorderedAccessView::UnorderedAccessView()
{
}

UnorderedAccessView::~UnorderedAccessView()
{
}

void UnorderedAccessView::CSSetUAV(const UINT& slot) const
{
	Renderer::context->CSSetUnorderedAccessViews(slot, 1, unorderedAccessView.GetAddressOf(), nullptr);
}
