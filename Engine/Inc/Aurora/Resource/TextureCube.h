#pragma once

#ifndef _TEXTURECUBE_H_
#define _TEXTURECUBE_H_

#include"ResourceTexture.h"
#include"RenderTexture.h"

#include<Aurora/Utils/Math.h>
#include<Aurora/Core/Shader.h>
#include<Aurora/EngineAPI/States.h>
#include<Aurora/EngineAPI/ImCtx.h>

class TextureCube :public Texture2D, public ShaderResourceView
{
public:

	TextureCube() = delete;

	//从6个贴图创建天空盒 xp xn yp yn zp zn
	TextureCube(std::initializer_list<std::string> texturesPath);

	//读取dds天空盒
	TextureCube(const std::string& texturePath);

	//从等距柱状图创建天空盒
	TextureCube(const std::string& texturePath, const UINT& skyboxResolution, const unsigned int& mipLevels = 1);

	~TextureCube();

	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

};

#endif // !_TEXTURECUBE_H_
