#pragma once

#include<Aurora/RenderTexture.h>
#include<Aurora/Texture2D.h>
#include<Aurora/Shader.h>
#include<Aurora/StateCommon.h>
#include<Aurora/Graphics.h>

class RealShading
{
public:

	RealShading() = delete;

	RealShading(const RealShading&) = delete;

	void operator=(const RealShading&) = delete;

	//只调用一次
	static Texture2D* getBRDF(const UINT& textureSize = 512);

};