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

	BloomEffect(const unsigned int& width, const unsigned int& height, const bool& enableBrightPixelExract = true);

	~BloomEffect();

	Texture2D* process(Texture2D* const texture2D) const override;

	Shader* bloomExtract;

	Shader* bloomFinal;

	Shader* blurShaders[blurSteps * 2];

	void setExposure(const float& exposure);

	void setGamma(const float& gamma);

	const float& getExposure() const;

	const float& getGamma() const;

	//更新向着色器传入的exposure和gamma
	void applyChange() const;

private:

	void compileShaders() override;

	DirectX::XMUINT2 resolutions[blurSteps];

	RenderTexture* renderTextures[blurSteps * 2];

	RenderTexture* originTexture;

	//明亮的像素会被提取到这个贴图上
	RenderTexture* bloomTexture;

	const unsigned int bloomWidth;

	const unsigned int bloomHeight;

	std::function<void(Texture2D* const)> firstPass;

	ComPtr<ID3D11Buffer> bloomParamBuffer;

	struct BloomParam
	{
		float exposure;
		float gamma;
		float v0[2];
	} bloomParam;

};

#endif // !_BLOOMEFFECT_H_