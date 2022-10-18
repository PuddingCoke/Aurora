﻿#include<Aurora/DX/Resource/Texture2D.h>

Texture2D::Texture2D(const std::string& path, const D3D11_USAGE& usage, const UINT& bindFlags, const UINT& cpuAccessFlag) :
	enableMSAA(false)
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
		mipLevels = 1;

		if (pixels)
		{
			tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			tDesc.ArraySize = 1;
			tDesc.MipLevels = 1;
			tDesc.BindFlags = bindFlags;
			tDesc.Width = (UINT)textureWidth;
			tDesc.Height = (UINT)textureHeight;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.Usage = usage;
			tDesc.CPUAccessFlags = cpuAccessFlag;

			subresource.pSysMem = pixels;
			subresource.SysMemPitch = width * 4u;

			Renderer::device->CreateTexture2D(&tDesc, &subresource, texture.ReleaseAndGetAddressOf());

			stbi_image_free(pixels);
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

		if (pixels)
		{
			tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			tDesc.ArraySize = 1;
			tDesc.MipLevels = 1;
			tDesc.BindFlags = bindFlags;
			tDesc.Width = (UINT)textureWidth;
			tDesc.Height = (UINT)textureHeight;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.Usage = usage;
			tDesc.CPUAccessFlags = cpuAccessFlag;

			subresource.pSysMem = pixels;
			subresource.SysMemPitch = width * 16u;

			Renderer::device->CreateTexture2D(&tDesc, &subresource, texture.ReleaseAndGetAddressOf());

			stbi_image_free(pixels);
		}
	}
	else if (fileExtension == "dds")
	{
		std::wstring wFilePath = std::wstring(path.begin(), path.end());

		DirectX::CreateDDSTextureFromFile(Renderer::device, wFilePath.c_str(), (ID3D11Resource**)texture.GetAddressOf(), nullptr);

		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);

		width = desc.Width;
		height = desc.Height;
		format = desc.Format;
		mipLevels = desc.MipLevels;
	}
	else
	{
		std::cout << "[class Texture2D] image format is not supported!\n";
	}

	std::cout << "[class Texture2D] " << path << " create successfully!\n";
}

Texture2D::Texture2D(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format, const D3D11_USAGE& usage, const UINT& bindFlags, const bool& enableMSAA, const UINT& cpuAccessFlag) :
	width(width), height(height), format(format), mipLevels(1), enableMSAA(enableMSAA)
{
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = width;
	tDesc.Height = height;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Format = format;
	tDesc.SampleDesc.Count = enableMSAA ? Graphics::getMSAALevel() : 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = usage;
	tDesc.BindFlags = bindFlags;
	tDesc.CPUAccessFlags = cpuAccessFlag;

	Renderer::device->CreateTexture2D(&tDesc, nullptr, texture.ReleaseAndGetAddressOf());
}

Texture2D::Texture2D(const unsigned int& width, const unsigned int& height, const TextureType& type) :
	width(width), height(height), format(DXGI_FORMAT_R32G32B32A32_FLOAT), mipLevels(1), enableMSAA(false)
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
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresource = {};
	subresource.pSysMem = colors.data();
	subresource.SysMemPitch = width * 16u;

	Renderer::device->CreateTexture2D(&tDesc, &subresource, texture.ReleaseAndGetAddressOf());
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

ID3D11Texture2D* Texture2D::getTexture2D() const
{
	return texture.Get();
}