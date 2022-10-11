#include<Aurora/ComputeTexture.h>

ComputeTexture::ComputeTexture(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const unsigned int& extraBindFlags) :
	Texture2D(width, height, format, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | extraBindFlags)
{

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	Renderer::device->CreateUnorderedAccessView(texture.Get(), &uavDesc, unorderedAccessView.ReleaseAndGetAddressOf());
}

ComputeTexture::~ComputeTexture()
{
}
