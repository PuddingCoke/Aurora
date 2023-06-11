#include<Aurora/Core/DX/View/DepthStencilView.h>

DepthStencilView::DepthStencilView()
{
}

DepthStencilView::DepthStencilView(ID3D11Resource* const resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc):
	DepthStencilView()
{
	createDSV(resource, desc);
}

void DepthStencilView::clearDSV(const UINT& clearFlag, const float& depth, const UINT8& stencil) const
{
	Renderer::getContext()->ClearDepthStencilView(depthStencilView.Get(), clearFlag, depth, stencil);
}

ID3D11DepthStencilView* DepthStencilView::getDSV() const
{
	return depthStencilView.Get();
}

DepthStencilView::~DepthStencilView()
{
}

void DepthStencilView::bindDSV()
{
}

void DepthStencilView::createDSV(ID3D11Resource* const resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc)
{
	Renderer::get()->createDepthStencilView(resource, &desc, depthStencilView.ReleaseAndGetAddressOf());
}
