#include<Aurora/Texture2D.h>

Texture2D* Texture2D::create(const std::string& path)
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

Texture2D::Texture2D(const std::string& path) :
	poolIndex(-1)
{
	D3D11_TEXTURE2D_DESC tDesc = {};

	D3D11_SUBRESOURCE_DATA subresource = {};

	int textureWidth, textureHeight, channels;

	std::string fileExtension = Utils::File::getExtension(path);

	for (char& c : fileExtension)
	{
		c = std::tolower(c);
	}

	if (fileExtension == "jpg" || fileExtension == "jpeg" || fileExtension == "png")
	{
		unsigned char* pixels = stbi_load(path.c_str(), &textureWidth, &textureHeight, &channels, 4);

		width = textureWidth;
		height = textureHeight;
		format = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (pixels)
		{
			tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			tDesc.ArraySize = 1;
			tDesc.MipLevels = 1;
			tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			tDesc.Width = (UINT)textureWidth;
			tDesc.Height = (UINT)textureHeight;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.Usage = D3D11_USAGE_IMMUTABLE;
			
			subresource.pSysMem = pixels;
			subresource.SysMemPitch = width * 4u;
			
			Graphics::device->CreateTexture2D(&tDesc, &subresource, texture2D.ReleaseAndGetAddressOf());

			stbi_image_free(pixels);

			createShaderResource();
		}
	}
	else if (fileExtension == "hdr")
	{
		float* pixels = stbi_loadf(path.c_str(), &textureWidth, &textureHeight, &channels, 0);

		std::cout << channels << "\n";

		stbi_image_free(pixels);
	}

	std::cout << "[class Texture2D] " << path << " create successfully!\n";
}

Texture2D::Texture2D(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const UINT& bindFlags) :
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

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		createShaderResource();
	}
}

void Texture2D::createShaderResource()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	Graphics::device->CreateShaderResourceView(texture2D.Get(), &srvDesc, resourceView.ReleaseAndGetAddressOf());
}
