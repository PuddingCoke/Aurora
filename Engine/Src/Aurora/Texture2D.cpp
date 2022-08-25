#include<Aurora/Texture2D.h>

Texture2D* Texture2D::create(const std::string& path)
{
	return new Texture2D(path, D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, (D3D11_CPU_ACCESS_FLAG)0);
}

Texture2D* Texture2D::create(const std::string& path, const D3D11_USAGE& usage, const D3D11_BIND_FLAG& bindFlag, const D3D11_CPU_ACCESS_FLAG& cpuAccessFlag)
{
	return new Texture2D(path, usage, bindFlag, cpuAccessFlag);
}

Texture2D* Texture2D::create(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const UINT& bindFlags)
{
	return new Texture2D(width, height, format, bindFlags);
}

Texture2D* Texture2D::createNoise(const unsigned int& width, const unsigned int& height, const bool& allowNegative)
{
	return new Texture2D(width, height, allowNegative);
}

Texture2D* Texture2D::createRandNormal(const unsigned int& width, const unsigned& height)
{
	return new Texture2D(width, height);
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

const DXGI_FORMAT& Texture2D::getFormat() const
{
	return format;
}

void Texture2D::setSRV(const UINT& slot) const
{
	Renderer::context->PSSetShaderResources(slot, 1, resourceView.GetAddressOf());
}

ID3D11ShaderResourceView* Texture2D::getSRV() const
{
	return resourceView.Get();
}

ID3D11Texture2D* Texture2D::getTexture2D() const
{
	return texture2D.Get();
}

Texture2D::Texture2D(const std::string& path, const D3D11_USAGE& usage, const D3D11_BIND_FLAG& bindFlag, const D3D11_CPU_ACCESS_FLAG& cpuAccessFlag) :
	poolIndex(-1)
{
	D3D11_TEXTURE2D_DESC tDesc = {};

	D3D11_SUBRESOURCE_DATA subresource = {};

	std::string fileExtension = Utils::File::getExtension(path);

	for (char& c : fileExtension)
	{
		c = std::tolower(c);
	}

	if (fileExtension == "jpg" || fileExtension == "jpeg" || fileExtension == "png")
	{
		int textureWidth, textureHeight, channels;

		unsigned char* pixels = stbi_load(path.c_str(), &textureWidth, &textureHeight, &channels, 4);

		width = textureWidth;
		height = textureHeight;
		format = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (pixels)
		{
			tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			tDesc.ArraySize = 1;
			tDesc.MipLevels = 1;
			tDesc.BindFlags = bindFlag;
			tDesc.Width = (UINT)textureWidth;
			tDesc.Height = (UINT)textureHeight;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.Usage = usage;
			tDesc.CPUAccessFlags = cpuAccessFlag;

			subresource.pSysMem = pixels;
			subresource.SysMemPitch = width * 4u;

			Renderer::device->CreateTexture2D(&tDesc, &subresource, texture2D.ReleaseAndGetAddressOf());

			stbi_image_free(pixels);

			if (bindFlag & D3D11_BIND_SHADER_RESOURCE)
			{
				createShaderResource();
			}
		}
	}
	else if (fileExtension == "hdr")
	{
		int textureWidth, textureHeight, channels;

		float* pixels = stbi_loadf(path.c_str(), &textureWidth, &textureHeight, &channels, 4);

		width = textureWidth;
		height = textureHeight;
		format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		if (pixels)
		{
			tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			tDesc.ArraySize = 1;
			tDesc.MipLevels = 1;
			tDesc.BindFlags = bindFlag;
			tDesc.Width = (UINT)textureWidth;
			tDesc.Height = (UINT)textureHeight;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.Usage = usage;
			tDesc.CPUAccessFlags = cpuAccessFlag;

			subresource.pSysMem = pixels;
			subresource.SysMemPitch = width * 16u;

			Renderer::device->CreateTexture2D(&tDesc, &subresource, texture2D.ReleaseAndGetAddressOf());

			stbi_image_free(pixels);

			if (bindFlag & D3D11_BIND_SHADER_RESOURCE)
			{
				createShaderResource();
			}
		}
	}
	else if (fileExtension == "dds")
	{
		std::wstring wFilePath = std::wstring(path.begin(), path.end());

		DirectX::CreateDDSTextureFromFile(Renderer::device.Get(), wFilePath.c_str(), nullptr, resourceView.GetAddressOf());

		ID3D11Resource* resource;

		resourceView->GetResource(&resource);

		resource->QueryInterface(IID_ID3D11Texture2D, (void**)texture2D.ReleaseAndGetAddressOf());

		texture2D->GetDesc(&tDesc);

		width = tDesc.Width;
		height = tDesc.Height;
		format = tDesc.Format;
	}
	else
	{
		std::cout << "[class Texture2D] image format is not supported!\n";
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

	Renderer::device->CreateTexture2D(&tDesc, nullptr, texture2D.ReleaseAndGetAddressOf());

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		createShaderResource();
	}
}

Texture2D::Texture2D(const unsigned int& width, const unsigned int& height, const bool& allowNegative) :
	poolIndex(-1), width(width), height(height), format(DXGI_FORMAT_R32G32B32A32_FLOAT)
{
	std::vector<DirectX::XMFLOAT4> colors(width * height);

	std::cout << "[class Texture2D] generate " << width << "x" << height << " noise texture\n";

	if (allowNegative)
	{
		for (unsigned int i = 0; i < width * height; i++)
		{
			colors[i] = DirectX::XMFLOAT4(Random::Float() * 2.f - 1.f, Random::Float() * 2.f - 1.f, Random::Float() * 2.f - 1.f, Random::Float() * 2.f - 1.f);
		}
	}
	else
	{
		for (unsigned int i = 0; i < width * height; i++)
		{
			colors[i] = DirectX::XMFLOAT4(Random::Float(), Random::Float(), Random::Float(), Random::Float());
		}
	}

	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = width;
	tDesc.Height = height;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Format = format;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresource = {};
	subresource.pSysMem = colors.data();
	subresource.SysMemPitch = width * 16u;

	Renderer::device->CreateTexture2D(&tDesc, &subresource, texture2D.ReleaseAndGetAddressOf());

	createShaderResource();
}

Texture2D::Texture2D(const unsigned int& width, const unsigned int& height) :
	poolIndex(-1), width(width), height(height), format(DXGI_FORMAT_R32G32B32A32_FLOAT)
{
	std::vector<DirectX::XMFLOAT4> colors(width * height);
	for (unsigned int i = 0; i < width * height; i++)
	{
		colors[i] = DirectX::XMFLOAT4(Random::Float(), Random::Float(), 1.f, 1.f);
	}

	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = width;
	tDesc.Height = height;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Format = format;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_IMMUTABLE;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresource = {};
	subresource.pSysMem = colors.data();
	subresource.SysMemPitch = width * 16u;

	Renderer::device->CreateTexture2D(&tDesc, &subresource, texture2D.ReleaseAndGetAddressOf());

	createShaderResource();
}

void Texture2D::createShaderResource()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	Renderer::device->CreateShaderResourceView(texture2D.Get(), &srvDesc, resourceView.ReleaseAndGetAddressOf());
}
