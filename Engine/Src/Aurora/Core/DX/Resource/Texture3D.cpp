#include<Aurora/Core/DX/Resource/Texture3D.h>

Texture3D::~Texture3D()
{
}

Texture3D::Texture3D(const UINT& width, const UINT& height, const UINT& depth, const FMT& format, const UINT& bindFlags, const UINT& miscFlags, const UINT& mipLevels) :
	width(width), height(height), depth(depth)
{
	D3D11_TEXTURE3D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.Format = FMTCAST(format);
	desc.BindFlags = bindFlags;
	desc.MiscFlags = miscFlags;
	desc.MipLevels = mipLevels;
	desc.Usage = D3D11_USAGE_DEFAULT;

	GraphicsDevice::get()->createTexture3D(&desc, nullptr, texture.ReleaseAndGetAddressOf());
}

ID3D11Resource* Texture3D::getResource() const
{
	return texture.Get();
}

ID3D11Texture3D* Texture3D::getTexture3D() const
{
	return texture.Get();
}
