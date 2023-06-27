#pragma once

#ifndef _SSREFFECT_H_
#define _SSREFFECT_H_

#include"EffectBase.h"

#include<Aurora/CompiledShaders/HiZCreateCS.h>
#include<Aurora/CompiledShaders/HiZInitializeCS.h>
#include<Aurora/CompiledShaders/HiZProcessPS.h>

class SSREffect :public EffectBase
{
public:

	SSREffect(GraphicsContext* const ctx, const UINT& width, const UINT& height);

	~SSREffect();

	//rg uv ba visibility
	ShaderResourceView* process(ShaderResourceView* resDepthTexture, ShaderResourceView* gPosition, ShaderResourceView* gNormal) const;

private:

	void compileShaders() override;

	Shader* hiZCreateCS;

	Shader* hiZInitializeCS;

	Shader* hiZProcessPS;

	ComputeTexture* hiZTexture;

	static constexpr UINT hiZMipLevel = 5;

};

#endif // !_SSREFFECT_H_
