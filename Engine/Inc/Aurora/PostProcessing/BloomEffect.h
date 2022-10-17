#pragma once

#ifndef _BLOOMEFFECT_H_
#define _BLOOMEFFECT_H_

#include"EffectBase.h"

// 关于如何实现的
//https://de45xmedrsdbp.cloudfront.net/Resources/files/The_Technology_Behind_the_Elemental_Demo_16x9-1248544805.pdf p60
//https://www.intel.com/content/www/us/en/developer/articles/technical/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms.html
//https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

class BloomEffect :public EffectBase
{
public:

	//这个参数是写死的
	static constexpr unsigned int blurSteps = 5;

	static constexpr DirectX::XMUINT2 workGroupSize = { 60,16 };

	BloomEffect(const unsigned int& width, const unsigned int& height);

	~BloomEffect();

	ShaderResourceView* process(ShaderResourceView* const texture2D) const;

	void setExposure(const float& exposure);

	void setGamma(const float& gamma);

	void setThreshold(const float& threshold);

	void setIntensity(const float& intensity);

	const float& getExposure() const;

	const float& getGamma() const;

	const float& getThreshold() const;

	const float& getIntensity() const;

	//更新向着色器传入的threshold intensity exposure gamma
	void applyChange() const;

private:

	void compileShaders() override;

	Shader* bloomExtract;

	Shader* bloomHBlurShader;

	Shader* bloomVBlurShader;

	Shader* bloomFinal;

	DirectX::XMUINT2 resolutions[blurSteps];

	RCTexture* rcTextures[blurSteps * 2];

	RenderTexture* originTexture;

	//明亮的像素会被提取到这个贴图上
	RenderTexture* bloomTexture;

	const unsigned int bloomWidth;

	const unsigned int bloomHeight;

	//float weight[4]
	//float offset[4]
	//float2 texelSize
	//int iteration
	//float v0
	ComPtr<ID3D11Buffer> blurParamBuffer[blurSteps];

	ComPtr<ID3D11ShaderResourceView> blurParamSRV[blurSteps];

	ComPtr<ID3D11Buffer> bloomParamBuffer;

	struct BloomParam
	{
		float exposure;
		float gamma;
		float threshold;
		float intensity;
	} bloomParam;

};

#endif // !_BLOOMEFFECT_H_