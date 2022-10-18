#pragma once

#ifndef _EFFECTBASE_H_
#define _EFFECTBASE_H_

#include<Aurora/RenderTexture.h>
#include<Aurora/RCTexture.h>
#include<Aurora/States.h>
#include<Aurora/Shader.h>
#include<Aurora/Graphics.h>
#include<Aurora/ResourceTexture.h>
#include<Aurora/RenderAPI.h>

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
