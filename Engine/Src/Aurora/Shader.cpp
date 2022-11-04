﻿#include<Aurora/Shader.h>

Shader* Shader::fullScreenVS;
Shader* Shader::fullScreenPS;

Shader::~Shader()
{
	(this->*releaseFunc)();
}

void Shader::use() const
{
	(this->*useFunc)();
}

const void* Shader::getBufferPointer() const
{
	return bufferPointer;
}

const size_t& Shader::getBufferSize() const
{
	return bufferSize;
}

void Shader::ini()
{
	fullScreenVS = new Shader(g_FullScreenVSBytes, sizeof(g_FullScreenVSBytes), ShaderType::Vertex);
	fullScreenPS = new Shader(g_FullScreenPSBytes, sizeof(g_FullScreenPSBytes), ShaderType::Pixel);
}

void Shader::release()
{
	delete fullScreenVS;
	delete fullScreenPS;
}

Shader::Shader(const std::string& filePath, const ShaderType& type, const std::initializer_list<D3D_SHADER_MACRO>& macros) :
	useFunc(nullptr), releaseFunc(nullptr)
{
	std::cout << "[class Shader] " << filePath;

	std::vector<D3D_SHADER_MACRO> shaderMacros = std::vector<D3D_SHADER_MACRO>{ macros };
	shaderMacros.push_back({ nullptr,nullptr });

	std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());

	switch (type)
	{
	default:
	case ShaderType::Vertex:
		D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "vs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		Renderer::device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shaderPtr.vertexShader);
		useFunc = &Shader::vertexUse;
		releaseFunc = &Shader::vertexRelease;
		break;
	case ShaderType::Hull:
		D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "hs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		Renderer::device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shaderPtr.hullShader);
		useFunc = &Shader::hullUse;
		releaseFunc = &Shader::hullRelease;
		break;
	case ShaderType::Domain:
		D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "ds_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		Renderer::device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shaderPtr.domainShader);
		useFunc = &Shader::domainUse;
		releaseFunc = &Shader::domainRelease;
		break;
	case ShaderType::Geometry:
		D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "gs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		Renderer::device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shaderPtr.geometryShader);
		useFunc = &Shader::geometryUse;
		releaseFunc = &Shader::geometryRelease;
		break;
	case ShaderType::Pixel:
		D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "ps_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		Renderer::device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shaderPtr.pixelShader);
		useFunc = &Shader::pixelUse;
		releaseFunc = &Shader::pixelRelease;
		break;
	case ShaderType::Compute:
		D3DCompileFromFile(wFilePath.c_str(), shaderMacros.data(), nullptr, "main", "cs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		Renderer::device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &shaderPtr.computeShader);
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
		Renderer::device->CreateVertexShader(bytes, byteSize, nullptr, &shaderPtr.vertexShader);
		useFunc = &Shader::vertexUse;
		releaseFunc = &Shader::vertexRelease;
		break;
	case ShaderType::Hull:
		Renderer::device->CreateHullShader(bytes, byteSize, nullptr, &shaderPtr.hullShader);
		useFunc = &Shader::hullUse;
		releaseFunc = &Shader::hullRelease;
		break;
	case ShaderType::Domain:
		Renderer::device->CreateDomainShader(bytes, byteSize, nullptr, &shaderPtr.domainShader);
		useFunc = &Shader::domainUse;
		releaseFunc = &Shader::domainRelease;
		break;
	case ShaderType::Geometry:
		Renderer::device->CreateGeometryShader(bytes, byteSize, nullptr, &shaderPtr.geometryShader);
		useFunc = &Shader::geometryUse;
		releaseFunc = &Shader::geometryRelease;
		break;
	case ShaderType::Pixel:
		Renderer::device->CreatePixelShader(bytes, byteSize, nullptr, &shaderPtr.pixelShader);
		useFunc = &Shader::pixelUse;
		releaseFunc = &Shader::pixelRelease;
		break;
	case ShaderType::Compute:
		Renderer::device->CreateComputeShader(bytes, byteSize, nullptr, &shaderPtr.computeShader);
		useFunc = &Shader::computeUse;
		releaseFunc = &Shader::computeRelease;
		break;
	}

	std::cout << " compile successfully!\n";
}

void Shader::vertexUse() const
{
	Renderer::context->VSSetShader(shaderPtr.vertexShader, nullptr, 0);
}

void Shader::hullUse() const
{
	Renderer::context->HSSetShader(shaderPtr.hullShader, nullptr, 0);
}

void Shader::domainUse() const
{
	Renderer::context->DSSetShader(shaderPtr.domainShader, nullptr, 0);
}

void Shader::geometryUse() const
{
	Renderer::context->GSSetShader(shaderPtr.geometryShader, nullptr, 0);
}

void Shader::pixelUse() const
{
	Renderer::context->PSSetShader(shaderPtr.pixelShader, nullptr, 0);
}

void Shader::computeUse() const
{
	Renderer::context->CSSetShader(shaderPtr.computeShader, nullptr, 0);
}

void Shader::vertexRelease()
{
	shaderPtr.vertexShader->Release();
}

void Shader::hullRelease()
{
	shaderPtr.hullShader->Release();
}

void Shader::domainRelease()
{
	shaderPtr.domainShader->Release();
}

void Shader::geometryRelease()
{
	shaderPtr.geometryShader->Release();
}

void Shader::pixelRelease()
{
	shaderPtr.pixelShader->Release();
}

void Shader::computeRelease()
{
	shaderPtr.computeShader->Release();
}
