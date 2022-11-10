#pragma once

#ifndef _SHADER_H_
#define _SHADER_H_

#include<d3d11_4.h>
#include<d3dcompiler.h>
#include<string>
#include<vector>
#include<iostream>

#include"Renderer.h"

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

	Shader(const std::string& filePath, const ShaderType& type, const std::initializer_list<D3D_SHADER_MACRO>& macros = {});

	Shader(const BYTE* const bytes, const size_t& byteSize, const ShaderType& type);

	void use() const;

	static constexpr UINT  compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

	const void* getBufferPointer() const;

	const size_t& getBufferSize() const;

private:

	friend class Aurora;

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

	void(Shader::* useFunc)() const;

	void(Shader::* releaseFunc)();

	void vertexUse() const;

	void hullUse() const;

	void domainUse() const;

	void geometryUse() const;

	void pixelUse() const;

	void computeUse() const;

	void vertexRelease();

	void hullRelease();

	void domainRelease();

	void geometryRelease();

	void pixelRelease();

	void computeRelease();

};

#define SHADERDATA(s) s->getBufferPointer(),s->getBufferSize()

#endif // !_SHADER_H_
