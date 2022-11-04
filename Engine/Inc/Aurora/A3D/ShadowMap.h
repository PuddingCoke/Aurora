#pragma once

#ifndef _SHADOWMAP_H_
#define _SHADOWMAP_H_

#include<Aurora/Shader.h>

#include<Aurora/DX/View/ShaderResourceView.h>
#include<Aurora/DX/View/DepthStencilView.h>

#include<Aurora/CompiledShaders/ShadowVS.h>

class ShadowMap :public DepthStencilView, public ShaderResourceView
{
public:

	ShadowMap() = delete;

	ShadowMap(const ShadowMap&) = delete;

	void operator=(const ShadowMap&) = delete;

	ShadowMap(const unsigned int& width, const unsigned int& height);

	void clear(const float& depth = 1.0f) const;

	static Shader* shadowVS;

	virtual void bindDSV() override;

private:

	friend class Aurora;

	static void ini();

	static void release();

};

#endif // !_SHADOWMAP_H_