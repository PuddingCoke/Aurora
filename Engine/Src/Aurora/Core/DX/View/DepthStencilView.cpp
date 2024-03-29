﻿#include<Aurora/Core/DX/View/DepthStencilView.h>

DepthStencilView::DepthStencilView()
{
}

ID3D11DepthStencilView* DepthStencilView::getDSV() const
{
	return depthStencilView.Get();
}

DepthStencilView::~DepthStencilView()
{
}

void DepthStencilView::bindDSV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
}

void DepthStencilView::createDSV(ID3D11Resource* const resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc)
{
	GraphicsDevice::get()->createDepthStencilView(resource, &desc, depthStencilView.ReleaseAndGetAddressOf());
}
