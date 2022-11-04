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

	//��6����ͼ������պ� xp xn yp yn zp zn
	TextureCube(std::initializer_list<std::string> texturesPath);

	//��ȡdds��պ�
	TextureCube(const std::string& texturePath);

	//�ӵȾ���״ͼ������պ�
	TextureCube(const std::string& texturePath, const UINT& skyboxResolution, const unsigned int& mipLevels = 1);

	~TextureCube();

	static Shader* skyboxVS;

private:

	friend class Aurora;

	static Shader* equirectangularVS;

	static Shader* equirectangularPS;

	static void iniShader();

	static void releaseShader();

	ComPtr<ID3D11Texture2D> cubeTexture;

};

#endif // !_TEXTURECUBE_H_
