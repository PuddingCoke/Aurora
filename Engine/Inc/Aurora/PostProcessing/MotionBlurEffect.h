#pragma once

#ifndef _MOTIONBLUREFFECT_H_
#define _MOTIONBLUREFFECT_H_

#include"EffectBase.h"

class MotionBlurEffect :public EffectBase
{
public:

	MotionBlurEffect

	void process(ShaderResourceView* const gPosition, ShaderResourceView* const depthView);


};

#endif // !_MOTIONBLUREFFECT_H_
