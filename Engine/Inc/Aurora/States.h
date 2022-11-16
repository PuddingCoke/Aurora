#pragma once

#ifndef _STATES_H_
#define _STATES_H_

#include"RenderAPI.h"

//这里定义了一些有用的状态
class States
{
public:

	static States* get();

	States(const States&) = delete;

	void operator=(const States&) = delete;

	static ID3D11SamplerState* anisotropicClampSampler;

	static ID3D11SamplerState* anisotropicWrapSampler;

	static ID3D11SamplerState* linearClampSampler;

	static ID3D11SamplerState* linearWrapSampler;

	static ID3D11SamplerState* pointClampSampler;

	static ID3D11SamplerState* pointWrapSampler;

	static ID3D11SamplerState* shadowSampler;

	//SRC_ALPHA INV_SRC_ALPHA
	static ID3D11BlendState* defBlendState;

	//ONE ONE ALPHA ONE ZERO
	static ID3D11BlendState* addtiveBlend;

	static ID3D11RasterizerState* rasterShadow;

	static ID3D11RasterizerState* rasterCullBack;

	static ID3D11RasterizerState* rasterCullFront;

	static ID3D11RasterizerState* rasterCullNone;

	//体素化很有用 8xMSAA + Conservative Rasterization
	static ID3D11RasterizerState2* rasterConserve;

	//depth less equal stencil disabled
	static ID3D11DepthStencilState* defDepthStencilState;

	static ID3D11DepthStencilState* depthStencilDisable;

private:

	friend class Aurora;

	static States* instance;

	States();

	~States();

};

#endif // !_STATES_H_
