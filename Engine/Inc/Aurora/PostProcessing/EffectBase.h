#pragma once

#ifndef _EFFECTBASE_H_
#define _EFFECTBASE_H_

#include<Aurora/RenderTexture.h>
#include<Aurora/RWTexture.h>
#include<Aurora/StateCommon.h>
#include<Aurora/Shader.h>
#include<Aurora/Graphics.h>

class EffectBase
{
public:

	EffectBase() = delete;

	EffectBase(const EffectBase&) = delete;

	void operator=(const EffectBase&) = delete;

	EffectBase(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& outputFormat);

	virtual ~EffectBase();

	Texture2D* getOutputTexture() const;

protected:

	virtual void compileShaders() = 0;

	RenderTexture* outputRTV;

	const unsigned int width;

	const unsigned int height;

};

#endif // !_EFFECTBASE_H_
