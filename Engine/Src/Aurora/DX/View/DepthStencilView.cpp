#include<Aurora/DX/View/DepthStencilView.h>

DepthStencilView::DepthStencilView(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const bool& enableMSAA, const UINT& arraySize)
{
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = width;
	tDesc.Height = height;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = arraySize;
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

	Renderer::device->CreateTexture2D(&tDesc, nullptr, depthStencilTexture.ReleaseAndGetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = tDesc.Format;

	if (arraySize == 1)
	{
		if (enableMSAA)
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}
	}
	else
	{
		if (enableMSAA)
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
			dsvDesc.Texture2DMSArray.ArraySize = arraySize;
			dsvDesc.Texture2DMSArray.FirstArraySlice = 0;
		}
		else
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.ArraySize = arraySize;
			dsvDesc.Texture2DArray.FirstArraySlice = 0;
			dsvDesc.Texture2DArray.MipSlice = 0;
		}
	}

	Renderer::device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, depthStencilView.ReleaseAndGetAddressOf());
}

void DepthStencilView::clear(const UINT& clearFlag, const float& depth, const UINT8& stencil) const
{
	Renderer::getContext()->ClearDepthStencilView(depthStencilView.Get(), clearFlag, depth, stencil);
}

ID3D11DepthStencilView* DepthStencilView::get() const
{
	return depthStencilView.Get();
}

DepthStencilView::~DepthStencilView()
{
}

void DepthStencilView::bindDSV()
{
}

DepthStencilView::DepthStencilView()
{
}
