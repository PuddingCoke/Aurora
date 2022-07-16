#include<Aurora/Texture2D.h>

Texture2D* Texture2D::create(const std::wstring& path)
{
	return new Texture2D(path);
}

Texture2D* Texture2D::create(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const UINT& bindFlags)
{
	return new Texture2D(width, height, format, bindFlags);
}

Texture2D::~Texture2D()
{
}

const unsigned int& Texture2D::getWidth() const
{
	return width;
}

const unsigned int& Texture2D::getHeight() const
{
	return height;
}

const float& Texture2D::getTexelSizeX() const
{
	return texelSizeX;
}

const float& Texture2D::getTexelSizeY() const
{
	return texelSizeY;
}

void Texture2D::setSRV(const UINT& slot) const
{
	Graphics::context->PSSetShaderResources(slot, 1, resourceView.GetAddressOf());
}

ID3D11ShaderResourceView* Texture2D::getSRV() const
{
	return resourceView.Get();
}

ID3D11Texture2D* Texture2D::getTexture2D() const
{
	return texture2D.Get();
}

Texture2D::Texture2D(const std::wstring& path) :
	poolIndex(-1)
{
	DirectX::CreateWICTextureFromFile(Graphics::device.Get(), path.c_str(), nullptr, resourceView.ReleaseAndGetAddressOf());

	ComPtr<ID3D11Resource> resource;

	resourceView->GetResource(resource.GetAddressOf());

	resource.As(&texture2D);

	D3D11_TEXTURE2D_DESC desc;

	texture2D->GetDesc(&desc);

	width = desc.Width;

	height = desc.Height;

	texelSizeX = 1.f / width;

	texelSizeY = 1.f / height;

	format = desc.Format;

	std::wcout << "[class Texture2D] " << path << " create successfully!\n";
}

Texture2D::Texture2D(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const UINT& bindFlags) :
	poolIndex(-1), width(width), height(height), format(format), texelSizeX(1.f / width), texelSizeY(1.f / height)
{
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = width;
	tDesc.Height = height;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Format = format;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = bindFlags;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;

	Graphics::device->CreateTexture2D(&tDesc, nullptr, texture2D.ReleaseAndGetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = tDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	Graphics::device->CreateShaderResourceView(texture2D.Get(), &srvDesc, resourceView.ReleaseAndGetAddressOf());
}
