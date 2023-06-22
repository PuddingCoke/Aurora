#pragma once

#ifndef _EFFECTBASE_H_
#define _EFFECTBASE_H_

#include<Aurora/EngineAPI/ResourceEssential.h>
#include<Aurora/EngineAPI/States.h>
#include<Aurora/EngineAPI/Camera.h>
#include<Aurora/EngineAPI/Graphics.h>
#include<Aurora/EngineAPI/ImCtx.h>

class EffectBase
{
public:

	EffectBase() = delete;

	EffectBase(const EffectBase&) = delete;

	void operator=(const EffectBase&) = delete;

	EffectBase(const unsigned int& width, const unsigned int& height, const FMT& outputFormat);

	virtual ~EffectBase();

protected:

	virtual void compileShaders() = 0;

	RenderTexture* outputRTV;

	const unsigned int width;

	const unsigned int height;

};

#endif // !_EFFECTBASE_H_
