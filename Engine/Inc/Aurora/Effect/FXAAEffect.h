#pragma once

#ifndef _FXAAEFFECT_H_
#define _FXAAEFFECT_H_

#include"EffectBase.h"

#include<Aurora/CompiledShaders/ColorToLuma.h>
#include<Aurora/CompiledShaders/FXAA.h>

#include<ImGUI/imgui.h>

//这个操作应该是最后加上的
class FXAAEffect :public EffectBase
{
public:

	FXAAEffect(GraphicsContext* const ctx, const UINT& width, const UINT& height);

	~FXAAEffect();

	//自动计算明亮值，所以只需要传入需要抗锯齿的材质就行了
	ShaderResourceView* process(ShaderResourceView* const texture2D) const;

	void setFXAAQualitySubpix(const float& exposure);

	void setFXAAQualityEdgeThreshold(const float& gamma);

	void setFXAAQualityEdgeThresholdMin(const float& threshold);

	//在class Game的imGUICall中调用此函数
	void imGUIEffectModifier();

	const float& getFXAAQualitySubpix() const;

	const float& getFXAAQualityEdgeThreshold() const;

	const float& getFXAAQualityEdgeThresholdMin() const;

	//更新向着色器传入的threshold intensity exposure gamma
	void applyChange() const;

	RenderTexture* colorWithLuma;

private:

	void compileShaders() override;

	struct FXAAParam
	{
		float fxaaQualityRcpFrame; // unused
		float fxaaQualitySubpix;
		float fxaaQualityEdgeThreshold;
		float fxaaQualityEdgeThresholdMin;
	} fxaaParam;

	ConstantBuffer* fxaaParamBuffer;

	Shader* colorToLuma;

	Shader* fxaaPS;

};

#endif // !_FXAAEFFECT_H_
