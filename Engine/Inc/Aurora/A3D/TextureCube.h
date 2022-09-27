#pragma once

#ifndef _TEXTURECUBE_H_
#define _TEXTURECUBE_H_

#include<Aurora/Texture2D.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/States.h>
#include<Aurora/Math.h>
#include<Aurora/Camera.h>
#include<Aurora/Shader.h>

class TextureCube
{
public:

	TextureCube() = delete;

	TextureCube(const TextureCube&) = delete;

	void operator=(const TextureCube&) = delete;

	//xp xn yp yn zp zn
	static TextureCube* create(std::initializer_list<std::string> texturesPath);

	//最好用这个方法载入天空盒和辐照图
	static TextureCube* createDDSCubeMap(const std::string& texturePath);

	//从hdr等距柱状投影图创建天空盒
	static TextureCube* createEquirectangularMap(const std::string& texturePath, const UINT& skyboxResolution, const DirectX::XMFLOAT3& up);

	void setSRV(const UINT& slot);

	~TextureCube();

	static Shader* shader;

private:

	friend class Aurora;

	static Shader* equirectangularYUP;

	static Shader* equirectangularZUP;

	static void iniShader();

	static void releaseShader();

	TextureCube(std::initializer_list<std::string> texturesPath);

	TextureCube(const std::string& texturePath);

	TextureCube(const std::string& texturePath, const UINT& skyboxResolution, const DirectX::XMFLOAT3& up);

	ComPtr<ID3D11Texture2D> cubeTexture;
	
	ComPtr<ID3D11ShaderResourceView> cubeSRV;
};

#endif // !_TEXTURECUBE_H_
