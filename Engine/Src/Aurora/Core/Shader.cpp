﻿#include<Aurora/Core/Shader.h>

Shader* Shader::fullScreenVS = nullptr;
Shader* Shader::fullScreenPS = nullptr;
Shader* Shader::skyboxVS = nullptr;
Shader* Shader::shadowVS = nullptr;
Shader* Shader::randNoiseCS = nullptr;
Shader* Shader::equirectangularVS = nullptr;
Shader* Shader::equirectangularPS = nullptr;

Shader::~Shader()
{
	(this->*releaseFunc)();
}

void Shader::use(ID3D11DeviceContext3* const ctx) const
{
	(this->*useFunc)(ctx);
}

const void* Shader::getBufferPointer() const
{
	return bufferPointer;
}

const size_t& Shader::getBufferSize() const
{
	return bufferSize;
}

void Shader::createGlobalShader()
{
	std::cout << "fullScreenVS ";
	fullScreenVS = new Shader(g_FullScreenVSBytes, sizeof(g_FullScreenVSBytes), ShaderType::Vertex);
	std::cout << "fullScreenPS ";
	fullScreenPS = new Shader(g_FullScreenPSBytes, sizeof(g_FullScreenPSBytes), ShaderType::Pixel);
	std::cout << "skyboxVS ";
	skyboxVS = new Shader(g_SkyboxVSBytes, sizeof(g_SkyboxVSBytes), ShaderType::Vertex);
	std::cout << "shadowVS ";
	shadowVS = new Shader(g_ShadowVSBytes, sizeof(g_ShadowVSBytes), ShaderType::Vertex);
	std::cout << "randNoiseCS ";
	randNoiseCS = new Shader(g_RandNoiseCSBytes, sizeof(g_RandNoiseCSBytes), ShaderType::Compute);
	std::cout << "equirectangularVS ";
	equirectangularVS = new Shader(g_EquirectangularVSBytes, sizeof(g_EquirectangularVSBytes), ShaderType::Vertex);
	std::cout << "equirectangularPS ";
	equirectangularPS = new Shader(g_EquirectangularPSBytes, sizeof(g_EquirectangularPSBytes), ShaderType::Pixel);
}

void Shader::releaseGlobalShader()
{
	delete fullScreenVS;
	delete fullScreenPS;
	delete skyboxVS;
	delete shadowVS;
	delete randNoiseCS;
	delete equirectangularVS;
	delete equirectangularPS;
}

Shader::Shader(const std::string& filePath, const ShaderType& type, const std::initializer_list<D3D_SHADER_MACRO>& macros) :
	useFunc(nullptr), releaseFunc(nullptr)
{
	std::cout << "[class Shader] " << filePath;

	const std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());

	const std::string fileExt = Utils::File::getExtension(filePath);

	if (fileExt == "hlsl")
	{
		std::vector<D3D_SHADER_MACRO> shaderMacros = std::vector<D3D_SHADER_MACRO>{ macros };
		shaderMacros.push_back({ nullptr,nullptr });

		switch (type)
		{
		default:
		case ShaderType::Vertex:
			D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "vs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
			break;
		case ShaderType::Hull:
			D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "hs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
			break;
		case ShaderType::Domain:
			D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "ds_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
			break;
		case ShaderType::Geometry:
			D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "gs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
			break;
		case ShaderType::Pixel:
			D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "ps_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
			break;
		case ShaderType::Compute:
			D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "cs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
			break;
		}
	}
	else if (fileExt == "cso")
	{
		D3DReadFileToBlob(wFilePath.c_str(), shaderBlob.ReleaseAndGetAddressOf());
	}
	else
	{
		throw "Unsupported file extension";
	}

	switch (type)
	{
	default:
	case ShaderType::Vertex:
		GraphicsDevice::get()->createVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &shaderPtr.vertexShader);
		useFunc = &Shader::vertexUse;
		releaseFunc = &Shader::vertexRelease;
		break;
	case ShaderType::Hull:
		GraphicsDevice::get()->createHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &shaderPtr.hullShader);
		useFunc = &Shader::hullUse;
		releaseFunc = &Shader::hullRelease;
		break;
	case ShaderType::Domain:
		GraphicsDevice::get()->createDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &shaderPtr.domainShader);
		useFunc = &Shader::domainUse;
		releaseFunc = &Shader::domainRelease;
		break;
	case ShaderType::Geometry:
		GraphicsDevice::get()->createGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &shaderPtr.geometryShader);
		useFunc = &Shader::geometryUse;
		releaseFunc = &Shader::geometryRelease;
		break;
	case ShaderType::Pixel:
		GraphicsDevice::get()->createPixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &shaderPtr.pixelShader);
		useFunc = &Shader::pixelUse;
		releaseFunc = &Shader::pixelRelease;
		break;
	case ShaderType::Compute:
		GraphicsDevice::get()->createComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &shaderPtr.computeShader);
		useFunc = &Shader::computeUse;
		releaseFunc = &Shader::computeRelease;
		break;
	}

	bufferPointer = shaderBlob->GetBufferPointer();
	bufferSize = shaderBlob->GetBufferSize();

	std::cout << " compile successfully!\n";
}

Shader::Shader(const BYTE* const bytes, const size_t& byteSize, const ShaderType& type) :
	bufferPointer(bytes), bufferSize(byteSize)
{
	std::cout << "[class Shader] Byte code";

	switch (type)
	{
	default:
	case ShaderType::Vertex:
		GraphicsDevice::get()->createVertexShader(bytes, byteSize, &shaderPtr.vertexShader);
		useFunc = &Shader::vertexUse;
		releaseFunc = &Shader::vertexRelease;
		break;
	case ShaderType::Hull:
		GraphicsDevice::get()->createHullShader(bytes, byteSize, &shaderPtr.hullShader);
		useFunc = &Shader::hullUse;
		releaseFunc = &Shader::hullRelease;
		break;
	case ShaderType::Domain:
		GraphicsDevice::get()->createDomainShader(bytes, byteSize, &shaderPtr.domainShader);
		useFunc = &Shader::domainUse;
		releaseFunc = &Shader::domainRelease;
		break;
	case ShaderType::Geometry:
		GraphicsDevice::get()->createGeometryShader(bytes, byteSize, &shaderPtr.geometryShader);
		useFunc = &Shader::geometryUse;
		releaseFunc = &Shader::geometryRelease;
		break;
	case ShaderType::Pixel:
		GraphicsDevice::get()->createPixelShader(bytes, byteSize, &shaderPtr.pixelShader);
		useFunc = &Shader::pixelUse;
		releaseFunc = &Shader::pixelRelease;
		break;
	case ShaderType::Compute:
		GraphicsDevice::get()->createComputeShader(bytes, byteSize, &shaderPtr.computeShader);
		useFunc = &Shader::computeUse;
		releaseFunc = &Shader::computeRelease;
		break;
	}

	std::cout << " compile successfully!\n";
}

void Shader::vertexUse(ID3D11DeviceContext3* const ctx) const
{
	ctx->VSSetShader(shaderPtr.vertexShader, nullptr, 0);
}

void Shader::hullUse(ID3D11DeviceContext3* const ctx) const
{
	ctx->HSSetShader(shaderPtr.hullShader, nullptr, 0);
}

void Shader::domainUse(ID3D11DeviceContext3* const ctx) const
{
	ctx->DSSetShader(shaderPtr.domainShader, nullptr, 0);
}

void Shader::geometryUse(ID3D11DeviceContext3* const ctx) const
{
	ctx->GSSetShader(shaderPtr.geometryShader, nullptr, 0);
}

void Shader::pixelUse(ID3D11DeviceContext3* const ctx) const
{
	ctx->PSSetShader(shaderPtr.pixelShader, nullptr, 0);
}

void Shader::computeUse(ID3D11DeviceContext3* const ctx) const
{
	ctx->CSSetShader(shaderPtr.computeShader, nullptr, 0);
}

void Shader::vertexRelease() const
{
	shaderPtr.vertexShader->Release();
}

void Shader::hullRelease() const
{
	shaderPtr.hullShader->Release();
}

void Shader::domainRelease() const
{
	shaderPtr.domainShader->Release();
}

void Shader::geometryRelease() const
{
	shaderPtr.geometryShader->Release();
}

void Shader::pixelRelease() const
{
	shaderPtr.pixelShader->Release();
}

void Shader::computeRelease() const
{
	shaderPtr.computeShader->Release();
}
