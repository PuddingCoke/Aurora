#pragma once

#ifndef _FADEEFFECT_H_
#define _FADEEFFECT_H_

#include"EffectBase.h"

class FadeEffect :public EffectBase
{
public:

	FadeEffect(const unsigned int& width, const unsigned int& height);

	Texture2D* process(Texture2D* const texture2D) const override;

	const float& getFadeFactor() const;

	void setFadeFactor(const float& factor);

	~FadeEffect();

protected:

	float fadeFactor;

	void compileShaders() override;

	Shader* fadePShader;

	ComPtr<ID3D11Buffer> fadeBuffer;

};

#endif // !_FADEEFFECT_H_

