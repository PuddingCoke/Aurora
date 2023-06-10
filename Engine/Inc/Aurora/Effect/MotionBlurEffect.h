#pragma once

#ifndef _MOTIONBLUREFFECT_H_
#define _MOTIONBLUREFFECT_H_

#include"EffectBase.h"

#include<Aurora/CompiledShaders/MotionBlurPS.h>

class MotionBlurEffect :public EffectBase
{
public:

	MotionBlurEffect(const unsigned int& width, const unsigned int& height, const FMT& format);

	~MotionBlurEffect();

	ShaderResourceView* process(ShaderResourceView* const gPosition, ShaderResourceView* const colorTexture);

private:

	void compileShaders() override;

	Shader* motionBlurPS;

};

#endif // !_MOTIONBLUREFFECT_H_
