#pragma once

#ifndef _FADEEFFECT_H_
#define _FADEEFFECT_H_

#include"EffectBase.h"

class FadeEffect :public EffectBase
{
public:

	FadeEffect(RenderTexture* defRenderTexture);

	void process() const override;

	~FadeEffect();

protected:

	void compileShaders() override;

	ComPtr<ID3D11BlendState> fadeBlendState;

	Shader* fadePShader;

};

#endif // !_FADEEFFECT_H_

