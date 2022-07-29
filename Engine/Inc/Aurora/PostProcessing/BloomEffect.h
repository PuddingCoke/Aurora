#pragma once

#include"EffectBase.h"

#ifndef _BLOOMEFFECT_H_
#define _BLOOMEFFECT_H_

// 关于如何实现的
//https://de45xmedrsdbp.cloudfront.net/Resources/files/The_Technology_Behind_the_Elemental_Demo_16x9-1248544805.pdf p60
//https://www.intel.com/content/www/us/en/developer/articles/technical/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms.html
//https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
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

#endif // !_BLOOMEFFECT_H_