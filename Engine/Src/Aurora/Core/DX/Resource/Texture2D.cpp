#include<Aurora/Core/DX/Resource/Texture2D.h>

Texture2D::Texture2D(const std::string& path) :
	width(0), height(0), format(DXGI_FORMAT_UNKNOWN), mipLevels(0), arraySize(0)
{
	D3D11_TEXTURE2D_DESC tDesc = {};

	D3D11_SUBRESOURCE_DATA subresource = {};

	std::string fileExtension = Utils::File::getExtension(path);

	for (char& c : fileExtension)
	{
		c = (char)std::tolower(c);
	}

	if (fileExtension == "jpg" || fileExtension == "jpeg" || fileExtension == "png")
	{
		int textureWidth, textureHeight, channels;

		unsigned char* pixels = stbi_load(path.c_str(), &textureWidth, &textureHeight, &channels, 4);

		width = textureWidth;
		height = textureHeight;
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
		mipLevels = 1;
		arraySize = 1;

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

			Renderer::getDevice()->CreateTexture2D(&tDesc, &subresource, texture.ReleaseAndGetAddressOf());

			stbi_image_free(pixels);
		}
		else
		{
			throw "Cannot open file named " + path;
		}
	}
	else if (fileExtension == "hdr")
	{
		int textureWidth, textureHeight, channels;

		float* pixels = stbi_loadf(path.c_str(), &textureWidth, &textureHeight, &channels, 4);

		width = textureWidth;
		height = textureHeight;
		format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		mipLevels = 1;
		arraySize = 1;

		if (pixels)
		{
			tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			tDesc.ArraySize = 1;
			tDesc.MipLevels = 1;
			tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			tDesc.Width = (UINT)textureWidth;
			tDesc.Height = (UINT)textureHeight;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.Usage = D3D11_USAGE_IMMUTABLE;

			subresource.pSysMem = pixels;
			subresource.SysMemPitch = width * 16u;

			Renderer::getDevice()->CreateTexture2D(&tDesc, &subresource, texture.ReleaseAndGetAddressOf());

			stbi_image_free(pixels);
		}
		else
		{
			throw "Cannot open file named " + path;
		}
	}
	else if (fileExtension == "dds")
	{
		std::wstring wFilePath = std::wstring(path.begin(), path.end());

		HRESULT hr = DirectX::CreateDDSTextureFromFile(Renderer::getDevice(), wFilePath.c_str(), (ID3D11Resource**)texture.GetAddressOf(), nullptr);

		if (hr == S_OK)
		{
			texture->GetDesc(&tDesc);

			width = tDesc.Width;
			height = tDesc.Height;
			format = tDesc.Format;
			mipLevels = tDesc.MipLevels;
			arraySize = tDesc.ArraySize;
		}
		else
		{
			throw "Cannot open file named " + path;
		}
	}
	else
	{
		std::cout << "[class Texture2D] image format is not supported!\n";
	}

	std::cout << "[class Texture2D] " << path << " create successfully!\n";
}

Texture2D::Texture2D(const UINT& width, const UINT& height, const TextureType& type) :
	width(width), height(height), format(DXGI_FORMAT_R32G32B32A32_FLOAT), mipLevels(1), arraySize(1)
{
	std::vector<DirectX::XMFLOAT4> colors(width * height);

	std::cout << "[class Texture2D] generate " << width << "x" << height << " ";

	switch (type)
	{
	default:
	case TextureType::Noise:
		std::cout << "noise";
		for (unsigned int i = 0; i < width * height; i++)
		{
			colors[i] = DirectX::XMFLOAT4(Random::Float(), Random::Float(), Random::Float(), Random::Float());
		}
		break;
	case TextureType::Gauss:
		std::cout << "gauss";
		for (unsigned int i = 0; i < width * height; i++)
		{
			colors[i] = DirectX::XMFLOAT4(Random::Gauss(), Random::Gauss(), Random::Gauss(), Random::Gauss());
		}
		break;
	}

	std::cout << " texture\n";

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

	D3D11_SUBRESOURCE_DATA subresource = {};
	subresource.pSysMem = colors.data();
	subresource.SysMemPitch = width * 16u;

	Renderer::getDevice()->CreateTexture2D(&tDesc, &subresource, texture.ReleaseAndGetAddressOf());
}

Texture2D::Texture2D(const UINT& width, const UINT& height, const DXGI_FORMAT& format, const UINT& bindFlags, const bool& enableMSAA) :
	width(width), height(height), format(format), mipLevels(1), arraySize(1)
{
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = width;
	tDesc.Height = height;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Format = format;
	tDesc.SampleDesc.Count = enableMSAA ? Graphics::getMSAALevel() : 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = bindFlags;

	Renderer::getDevice()->CreateTexture2D(&tDesc, nullptr, texture.ReleaseAndGetAddressOf());
}

Texture2D::Texture2D(const UINT& width, const UINT& height, const UINT& mipLevels, const UINT& arraySize, const DXGI_FORMAT& format, const UINT& bindFlags, const UINT& miscFlags) :
	width(width), height(height), format(format), mipLevels(mipLevels), arraySize(arraySize)
{
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = width;
	tDesc.Height = height;
	tDesc.MipLevels = mipLevels;
	tDesc.ArraySize = arraySize;
	tDesc.Format = format;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = bindFlags;
	tDesc.MiscFlags = miscFlags;

	Renderer::getDevice()->CreateTexture2D(&tDesc, nullptr, texture.ReleaseAndGetAddressOf());
}

Texture2D::~Texture2D()
{
}

const UINT& Texture2D::getWidth() const
{
	return width;
}

const UINT& Texture2D::getHeight() const
{
	return height;
}

const DXGI_FORMAT& Texture2D::getFormat() const
{
	return format;
}

const UINT& Texture2D::getMipLevels() const
{
	return mipLevels;
}

const UINT& Texture2D::getArraySize() const
{
	return arraySize;
}

ID3D11Resource* Texture2D::getResource() const
{
	return texture.Get();
}

ID3D11Texture2D* Texture2D::getTexture2D() const
{
	return texture.Get();
}