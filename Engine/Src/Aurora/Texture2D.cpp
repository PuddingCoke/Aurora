#include<Aurora/Texture2D.h>

Texture2D* Texture2D::create(const std::wstring& path)
{
	return new Texture2D(path);
}

Texture2D* Texture2D::create(ID3D11Texture2D* const texture)
{
	return new Texture2D(texture);
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

void Texture2D::setSRV(const UINT& slot) const
{
	Graphics::context->PSSetShaderResources(slot, 1, resourceView.GetAddressOf());
}

Texture2D::Texture2D():
	poolIndex(-1)
{
}

Texture2D::Texture2D(const std::wstring& path):
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

	format = desc.Format;

	std::wcout << "[class Texture2D] " << path << " create successfully!\n";
}

Texture2D::Texture2D(ID3D11Texture2D* const texture):
	poolIndex(-1),texture2D(texture)
{
	D3D11_TEXTURE2D_DESC desc;

	texture2D->GetDesc(&desc);

	width = desc.Width;

	height = desc.Height;

	format = desc.Format;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	Graphics::device->CreateShaderResourceView(texture, &srvDesc, resourceView.ReleaseAndGetAddressOf());
}

Texture2D::Texture2D(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const UINT& bindFlags):
	poolIndex(-1), width(width), height(height), format(format)
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
