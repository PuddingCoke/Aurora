#include<Aurora/DX/Resource/Texture3D.h>

Texture3D::~Texture3D()
{
}

Texture3D::Texture3D(const UINT& width, const UINT& height, const UINT& depth, const DXGI_FORMAT& format, const UINT& bindFlags, const UINT& miscFlags, const UINT& mipLevels) :
	width(width), height(height), depth(depth)
{
	D3D11_TEXTURE3D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.Format = format;
	desc.BindFlags = bindFlags;
	desc.MiscFlags = miscFlags;
	desc.MipLevels = mipLevels;
	desc.Usage = D3D11_USAGE_DEFAULT;

	Renderer::getDevice()->CreateTexture3D(&desc, nullptr, texture.ReleaseAndGetAddressOf());
}
