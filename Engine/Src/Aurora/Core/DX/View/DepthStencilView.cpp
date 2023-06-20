#include<Aurora/Core/DX/View/DepthStencilView.h>

DepthStencilView::DepthStencilView()
{
}

DepthStencilView::DepthStencilView(ID3D11Resource* const resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc):
	DepthStencilView()
{
	createDSV(resource, desc);
}

ID3D11DepthStencilView* DepthStencilView::getDSV() const
{
	return depthStencilView.Get();
}

DepthStencilView::~DepthStencilView()
{
}

void DepthStencilView::bindDSV(ID3D11DeviceContext3* const ctx)
{
}

void DepthStencilView::createDSV(ID3D11Resource* const resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc)
{
	GraphicsDevice::get()->createDepthStencilView(resource, &desc, depthStencilView.ReleaseAndGetAddressOf());
}
