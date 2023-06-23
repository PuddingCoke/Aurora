#pragma once

#ifndef _FADEEFFECT_H_
#define _FADEEFFECT_H_

#include"EffectBase.h"

#include<Aurora/CompiledShaders/FadePS.h>

class FadeEffect :public EffectBase
{
public:

	FadeEffect(GraphicsContext* const ctx, const unsigned int& width, const unsigned int& height);

	ShaderResourceView* process(ShaderResourceView* const texture2D) const;

	const float& getFadeSpeed() const;

	void setFadeSpeed(const float& factor);

	~FadeEffect();

protected:

	struct FadeParam
	{
		float fadeSpeed;
		float v0;
		float v1;
		float v2;
	} fadeParam;

	void compileShaders() override;

	Shader* fadePShader;

	ConstantBuffer* fadeBuffer;

};

#endif // !_FADEEFFECT_H_

