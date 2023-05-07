#pragma once

#ifndef _EFFECTBASE_H_
#define _EFFECTBASE_H_

#include<Aurora/Core/Resource/RenderTexture.h>
#include<Aurora/Core/Resource/RCTexture.h>
#include<Aurora/Core/Resource/ResourceTexture.h>
#include<Aurora/Core/States.h>
#include<Aurora/Core/Shader.h>
#include<Aurora/Core/Camera.h>
#include<Aurora/Core/Graphics.h>
#include<Aurora/Core/RenderAPI.h>

class EffectBase
{
public:

	EffectBase() = delete;

	EffectBase(const EffectBase&) = delete;

	void operator=(const EffectBase&) = delete;

	EffectBase(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& outputFormat);

	virtual ~EffectBase();

protected:

	virtual void compileShaders() = 0;

	RenderTexture* outputRTV;

	const unsigned int width;

	const unsigned int height;

};

#endif // !_EFFECTBASE_H_
