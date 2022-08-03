#pragma once

#include"EffectBase.h"

#ifndef _BLOOMEFFECT_H_
#define _BLOOMEFFECT_H_

// �������ʵ�ֵ�
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

	const float& getExposure();

	const float& getGamma();

	//��������ɫ�������exposure��gamma
	void applyChange();

private:

	void compileShaders() override;

	unsigned int resolutionX[blurSteps];

	unsigned int resolutionY[blurSteps];

	RenderTexture* renderTextures[blurSteps * 2];

	RenderTexture* originTexture;

	//���������ػᱻ��ȡ�������ͼ��
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