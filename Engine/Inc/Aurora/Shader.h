#pragma once

#ifndef _SHADER_H_
#define _SHADER_H_

#include<d3d11_4.h>
#include<d3dcompiler.h>
#include<string>
#include<iostream>
#include<vector>

#include"Utils.h"
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

	void use() const;

	ID3DBlob* getBlob() const;

	static Shader* fromFile(const std::string& filePath, const ShaderType& type, const std::initializer_list<D3D_SHADER_MACRO>& macros = {});

	static Shader* fromStr(const std::string& source, const ShaderType& type, const std::initializer_list<D3D_SHADER_MACRO>& macros = {});

	static constexpr UINT  compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

	static Shader* displayVShader;

	static Shader* displayPShader;

private:

	static void ini();

	static void release();

	friend class Aurora;

	ComPtr<ID3DBlob> shaderBlob;

	Shader(const std::string& source, const ShaderType& type, const std::initializer_list<D3D_SHADER_MACRO>& macros);

	ID3D11DeviceChild* shaderPtr;

	void(*useFunc)(ID3D11DeviceChild* const);

	void(*releaseFunc)(ID3D11DeviceChild* const);
};

#define SHADERDATA(s) s->getBlob()->GetBufferPointer(),s->getBlob()->GetBufferSize()

#endif // !_SHADER_H_
