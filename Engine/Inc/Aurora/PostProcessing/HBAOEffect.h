#pragma once

#ifndef _HBAOEFFECT_H_
#define _HBAOEFFECT_H_

#include<Aurora/Camera.h>
#include<Aurora/Math.h>

#include<GFSDK_SSAO/GFSDK_SSAO.h>

#include"EffectBase.h"

class HBAOEffect :public EffectBase
{
public:

	HBAOEffect() = delete;

	HBAOEffect(const HBAOEffect&) = delete;

	void operator=(const HBAOEffect&) = delete;

	HBAOEffect(const unsigned int& width, const unsigned int& height);

	~HBAOEffect();

	Texture2D* process(ID3D11ShaderResourceView* const depthSRV, ID3D11ShaderResourceView* const normalSRV) const;

	const float& getRadius() const;

	const float& getBias() const;

	const float& getPowerExponent() const;

	const float& getBlurSharpness() const;

	void setRadius(const float& radius);

	void setBias(const float& bias);

	void setPowerExponent(const float& powerExponent);

	void setBlurSharpness(const float& blurSharpness);

private:

	void compileShaders() override;

	GFSDK_SSAO_Context_D3D11* pAOContext;

	GFSDK_SSAO_CustomHeap customHeap;

	float radius;

	float bias;

	float powerExponent;

	float blurSharpness;

};

#endif // !_HBAOEFFECT_H_