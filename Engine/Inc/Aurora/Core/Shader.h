﻿#pragma once

#ifndef _SHADER_H_
#define _SHADER_H_

#include<d3d11_4.h>
#include<d3dcompiler.h>
#include<string>
#include<vector>
#include<iostream>

#include"Renderer.h"
#include<Aurora/Utils/Utils.h>

enum class ShaderType
{
	Vertex,
	Hull,
	Domain,
	Geometry,
	Pixel,
	Compute
};

class Shader
{
public:

	Shader() = delete;

	Shader(const Shader&) = delete;

	~Shader();

	void operator=(const Shader&) = delete;

	//hlsl or cso
	Shader(const std::string& filePath, const ShaderType& type, const std::initializer_list<D3D_SHADER_MACRO>& macros = {});

	//byte code
	Shader(const BYTE* const bytes, const size_t& byteSize, const ShaderType& type);

	static constexpr UINT  compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

	const void* getBufferPointer() const;

	const size_t& getBufferSize() const;

private:

	friend class ImCtx;

	ComPtr<ID3DBlob> shaderBlob;

	const void* bufferPointer;

	size_t bufferSize;

	union ShaderPRT
	{
		ID3D11VertexShader* vertexShader;
		ID3D11HullShader* hullShader;
		ID3D11DomainShader* domainShader;
		ID3D11GeometryShader* geometryShader;
		ID3D11PixelShader* pixelShader;
		ID3D11ComputeShader* computeShader;
	} shaderPtr;

	void use(ID3D11DeviceContext3* const ctx) const;

	void(Shader::* useFunc)(ID3D11DeviceContext3* const ctx) const;

	void(Shader::* releaseFunc)() const;

	void vertexUse(ID3D11DeviceContext3* const ctx) const;

	void hullUse(ID3D11DeviceContext3* const ctx) const;

	void domainUse(ID3D11DeviceContext3* const ctx) const;

	void geometryUse(ID3D11DeviceContext3* const ctx) const;

	void pixelUse(ID3D11DeviceContext3* const ctx) const;

	void computeUse(ID3D11DeviceContext3* const ctx) const;

	void vertexRelease() const;

	void hullRelease() const;

	void domainRelease() const;

	void geometryRelease() const;

	void pixelRelease() const;

	void computeRelease() const;

};

#define SHADERDATA(s) s->getBufferPointer(),s->getBufferSize()

#endif // !_SHADER_H_
