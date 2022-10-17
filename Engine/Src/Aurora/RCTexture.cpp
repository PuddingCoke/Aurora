#include<Aurora/RCTexture.h>

RCTexture::RCTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const float color[4]) :
	Texture2D(width, height, format, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = mipLevels;

		Renderer::device->CreateShaderResourceView(texture.Get(), &srvDesc, shaderResourceView.ReleaseAndGetAddressOf());
	}

	{
		D3D11_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = format;
		viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateRenderTargetView(texture.Get(), &viewDesc, renderTargetView.ReleaseAndGetAddressOf());
	}

	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateUnorderedAccessView(texture.Get(), &uavDesc, unorderedAccessView.ReleaseAndGetAddressOf());
	}

	Renderer::context->ClearRenderTargetView(renderTargetView.Get(), color);
}

RCTexture::~RCTexture()
{
}
