#pragma once

#include<Aurora/RenderTexture.h>
#include<Aurora/Texture2D.h>
#include<Aurora/Shader.h>
#include<Aurora/StateCommon.h>
#include<Aurora/Graphics.h>

class RealShading
{
public:

	//ֻ����һ��
	static Texture2D* getSmithBRDF(const UINT& textureSize = 512);

};