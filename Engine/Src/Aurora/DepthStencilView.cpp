﻿#include<Aurora/DepthStencilView.h>

DepthStencilView* DepthStencilView::create(const DXGI_FORMAT& format, const bool& enableMSAA)
{
	return new DepthStencilView(format, enableMSAA);
}

void DepthStencilView::clear(const UINT& clearFlag, const float& depth, const UINT8& stencil) const
{
	Graphics::context->ClearDepthStencilView(depthStencilView.Get(), clearFlag, depth, stencil);
}

ID3D11DepthStencilView* DepthStencilView::get() const
{
	return depthStencilView.Get();
}

DepthStencilView::~DepthStencilView()
{
}

DepthStencilView::DepthStencilView(const DXGI_FORMAT& format, const bool& enableMSAA)
{
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = Graphics::getWidth();
	tDesc.Height = Graphics::getHeight();
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Format = format;
	if (enableMSAA)
	{
		tDesc.SampleDesc.Count = Graphics::getMSAALevel();
	}
	else
	{
		tDesc.SampleDesc.Count = 1;
	}
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;

	Graphics::device->CreateTexture2D(&tDesc, nullptr, depthStencilTexture.ReleaseAndGetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = tDesc.Format;

	if (enableMSAA)
	{
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
	}

	dsvDesc.Texture2D.MipSlice = 0;

	Graphics::device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, depthStencilView.ReleaseAndGetAddressOf());
}