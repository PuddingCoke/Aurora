#pragma once

#ifndef _BLOOMEFFECT_H_
#define _BLOOMEFFECT_H_

#include"EffectBase.h"

#include<Aurora/CompiledShaders/BloomExtractPS.h>
#include<Aurora/CompiledShaders/BloomFinalPS.h>
#include<Aurora/CompiledShaders/BloomVBlurCS.h>
#include<Aurora/CompiledShaders/BloomHBlurCS.h>
#include<Aurora/CompiledShaders/BloomDownSamplePS.h>

#include<Aurora/StructuredBuffer.h>
#include<Aurora/Math.h>

#include<ImGUI/imgui.h>

// 关于如何实现的
//https://de45xmedrsdbp.cloudfront.net/Resources/files/The_Technology_Behind_the_Elemental_Demo_16x9-1248544805.pdf p60
//https://www.intel.com/content/www/us/en/developer/articles/technical/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms.html
//https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

class BloomEffect :public EffectBase
{
public:

	//这个参数是写死的
	static constexpr unsigned int blurSteps = 5;

	//随着分辨率的降低模糊半径逐渐增大
	static constexpr unsigned int iteration[blurSteps] = { 2,3,4,5,6 };

	static constexpr DirectX::XMUINT2 workGroupSize = { 60,16 };

	BloomEffect(const unsigned int& width, const unsigned int& height);

	~BloomEffect();

	ShaderResourceView* process(ShaderResourceView* const texture2D) const;

	void setExposure(const float& exposure);

	void setGamma(const float& gamma);

	void setThreshold(const float& threshold);

	void setIntensity(const float& intensity);

	//在class Game的imGUICall中调用此函数
	void imGUIBloomEffectModifier();

	const float& getExposure() const;

	const float& getGamma() const;

	const float& getThreshold() const;

	const float& getIntensity() const;

	//更新向着色器传入的threshold intensity exposure gamma
	void applyChange() const;

private:

	void compileShaders() override;

	//修改模糊曲线
	void updateCurve(const unsigned int& index);

	Shader* bloomExtract;

	Shader* bloomHBlur;

	Shader* bloomVBlur;

	Shader* bloomFinal;

	Shader* bloomDownSample;

	DirectX::XMUINT2 resolutions[blurSteps];

	RCTexture* rcTextures[blurSteps * 2];

	RenderTexture* originTexture;

	//明亮的像素会被提取到这个材质上
	RenderTexture* bloomTexture;

	const unsigned int bloomWidth;

	const unsigned int bloomHeight;

	StructuredBuffer* blurParamBuffer[blurSteps];

	Buffer* bloomParamBuffer;

	struct BloomParam
	{
		float exposure;
		float gamma;
		float threshold;
		float intensity;
	}bloomParam;

	struct BlurParam
	{
		float weight[8];
		float offset[8];
		DirectX::XMFLOAT2 texelSize;
		unsigned int iteration;
		float sigma;
	}blurParam[blurSteps]{};

};

#endif // !_BLOOMEFFECT_H_