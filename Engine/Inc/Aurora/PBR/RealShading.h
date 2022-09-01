#pragma once

#include<Aurora/RenderTexture.h>
#include<Aurora/Texture2D.h>
#include<Aurora/Shader.h>
#include<Aurora/StateCommon.h>
#include<Aurora/Graphics.h>

class RealShading
{
public:

	//只调用一次
	static Texture2D* getBRDF(const UINT& textureSize = 512);

};