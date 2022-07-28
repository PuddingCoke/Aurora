#pragma once

#include"EffectBase.h"

class BloomEffect :public EffectBase
{
public:

	static constexpr unsigned int blurSteps = 5;

	BloomEffect(const unsigned int& width, const unsigned int& height);

	~BloomEffect();

	Texture2D* process(Texture2D* const texture2D) const override;

	Shader* bloomExtract;

	Shader* bloomFinal;

	Shader* blurShaders[blurSteps * 2];

private:

	void compileShaders() override;

	unsigned int resolutionX[blurSteps];

	unsigned int resolutionY[blurSteps];

	RenderTexture* renderTextures[blurSteps * 2];

	RenderTexture* originTexture;

	//明亮的像素会被提取到这个贴图上
	RenderTexture* bloomTexture;

	const unsigned int bloomWidth;

	const unsigned int bloomHeight;

};