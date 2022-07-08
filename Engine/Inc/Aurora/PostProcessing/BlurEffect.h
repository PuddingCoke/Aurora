#pragma once

#ifndef _BLUREFFECT_H_
#define _BLUREFFECT_H_

#include<Aurora/Shader.h>

#include"EffectBase.h"

class BlurEffect:public EffectBase
{
public:

	BlurEffect(const unsigned int& width, const unsigned int& height);

	~BlurEffect();

	virtual Texture2D* process(Texture2D* const texture2D) const override;

	virtual void compileShaders() override;
	
private:

	RenderTexture* renderTexture;

};

#endif // !_BLUREFFECT_H_
