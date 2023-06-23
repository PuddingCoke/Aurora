#pragma once

#ifndef _EFFECTBASE_H_
#define _EFFECTBASE_H_

#include<Aurora/EngineAPI/States.h>
#include<Aurora/EngineAPI/Camera.h>
#include<Aurora/EngineAPI/Graphics.h>
#include<Aurora/EngineAPI/ResourceEssential.h>

class EffectBase
{
public:

	EffectBase() = delete;

	EffectBase(const EffectBase&) = delete;

	void operator=(const EffectBase&) = delete;

	EffectBase(GraphicsContext* const ctx, const UINT& width, const UINT& height, const FMT& outputFormat);

	virtual ~EffectBase();

protected:

	virtual void compileShaders() = 0;

	RenderTexture* outputRTV;

	GraphicsContext* ctx;

	const UINT width;

	const UINT height;

};

#endif // !_EFFECTBASE_H_
