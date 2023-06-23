#pragma once

#ifndef _FXAAEFFECT_H_
#define _FXAAEFFECT_H_

#include"EffectBase.h"

#include<Aurora/CompiledShaders/ColorToLuma.h>
#include<Aurora/CompiledShaders/FXAA.h>

#include<ImGUI/imgui.h>

//�������Ӧ���������ϵ�
class FXAAEffect :public EffectBase
{
public:

	FXAAEffect(GraphicsContext* const ctx, const UINT& width, const UINT& height);

	~FXAAEffect();

	//�Զ���������ֵ������ֻ��Ҫ������Ҫ����ݵĲ��ʾ�����
	ShaderResourceView* process(ShaderResourceView* const texture2D) const;

	void setFXAAQualitySubpix(const float& exposure);

	void setFXAAQualityEdgeThreshold(const float& gamma);

	void setFXAAQualityEdgeThresholdMin(const float& threshold);

	//��class Game��imGUICall�е��ô˺���
	void imGUIEffectModifier();

	const float& getFXAAQualitySubpix() const;

	const float& getFXAAQualityEdgeThreshold() const;

	const float& getFXAAQualityEdgeThresholdMin() const;

	//��������ɫ�������threshold intensity exposure gamma
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
