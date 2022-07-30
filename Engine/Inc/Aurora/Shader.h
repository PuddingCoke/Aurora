#pragma once

#ifndef _SHADER_H_
#define _SHADER_H_

#include<d3d11_4.h>
#include<d3dcompiler.h>
#include<wrl/client.h>
#include<string>
#include<iostream>
#include<functional>

#include"Utils.h"
#include"StateCommon.h"

enum class ShaderType
{
	Compute,
	Domain,
	Geometry,
	Hull,
	Pixel,
	Vertex
};

class Shader
{
public:

	Shader() = delete;

	Shader(const Shader&) = delete;

	~Shader();

	void operator=(const Shader&) = delete;

	ComPtr<ID3DBlob> shaderBlob;

	ID3D11DeviceChild* shaderPtr;

	void use();

	static Shader* fromFile(const std::string& filePath, const ShaderType& type);

	static Shader* fromStr(const std::string& source, const ShaderType& type);

	static constexpr UINT  compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

	static Shader* displayVShader;

	static Shader* displayPShader;

private:

	static void ini();

	static void release();

	friend class Aurora;

	Shader(const std::string& source, const ShaderType& type);

	std::function<void(void)> useFunc;

	std::function<void(void)> releaseFunc;
};

#endif // !_SHADER_H_
