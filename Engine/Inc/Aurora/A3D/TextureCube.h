#pragma once

#ifndef _TEXTURECUBE_H_
#define _TEXTURECUBE_H_

#include<Aurora/ResourceTexture.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/States.h>
#include<Aurora/Math.h>
#include<Aurora/Camera.h>
#include<Aurora/Shader.h>
#include<Aurora/RenderAPI.h>
#include<Aurora/DX/View/ShaderResourceView.h>

#include<Aurora/CompiledShaders/SkyboxVS.h>
#include<Aurora/CompiledShaders/EquirectangularVS.h>
#include<Aurora/CompiledShaders/EquirectangularPS.h>

class TextureCube :public ShaderResourceView
{
public:

	TextureCube() = delete;

	TextureCube(const TextureCube&) = delete;

	void operator=(const TextureCube&) = delete;

	//xp xn yp yn zp zn
	static TextureCube* create(std::initializer_list<std::string> texturesPath);

	//最好用这个方法载入天空盒和辐照图
	static TextureCube* createDDSCubeMap(const std::string& texturePath);

	//从hdr等距柱状投影图创建天空盒 mipLevels主要用于IBL
	static TextureCube* createEquirectangularMap(const std::string& texturePath, const UINT& skyboxResolution, const unsigned int& mipLevels = 1);

	~TextureCube();

	static Shader* skyboxVS;

private:

	friend class Aurora;

	static Shader* equirectangularVS;

	static Shader* equirectangularPS;

	static void iniShader();

	static void releaseShader();

	TextureCube(std::initializer_list<std::string> texturesPath);

	TextureCube(const std::string& texturePath);

	TextureCube(const std::string& texturePath, const UINT& skyboxResolution, const unsigned int& mipLevels);

	ComPtr<ID3D11Texture2D> cubeTexture;

};

#endif // !_TEXTURECUBE_H_
