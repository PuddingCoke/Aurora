#include<Aurora/DX/Resource/Texture3D.h>

Texture3D::~Texture3D()
{
}

Texture3D::Texture3D(const UINT& width, const UINT& height, const UINT& depth, const DXGI_FORMAT& format, const UINT& bindFlags) :
	width(width), height(height), depth(depth), format(format)
{
	D3D11_TEXTURE3D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.Format = format;
	desc.BindFlags = bindFlags;
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;

	Renderer::device->CreateTexture3D(&desc, nullptr, texture.ReleaseAndGetAddressOf());
}
