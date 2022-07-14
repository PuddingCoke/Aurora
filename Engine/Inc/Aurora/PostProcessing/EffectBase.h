#pragma once

#ifndef _EFFECTBASE_H_
#define _EFFECTBASE_H_

#include<Aurora/RenderTexture.h>
#include<Aurora/StateCommon.h>
#include<Aurora/Shader.h>

class EffectBase
{
public:

	EffectBase() = delete;

	EffectBase(const EffectBase&) = delete;

	void operator=(const EffectBase&) = delete;

	EffectBase(RenderTexture* defRenderTexture);

	EffectBase(const unsigned int& width, const unsigned int& height);

	virtual ~EffectBase();

	virtual Texture2D* process(Texture2D* const texture2D) const;

	virtual void process() const;

	Texture2D* getOutputTexture() const;

protected:

	virtual void compileShaders() = 0;

	Texture2D* outputTexture;

	RenderTexture* defRenderTexture;

	const unsigned int width;

	const unsigned int height;

};

#endif // !_EFFECTBASE_H_
