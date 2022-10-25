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

	ComPtr<ID3D11SamplerState> linearClampSampler;

	ComPtr<ID3D11SamplerState> linearWrapSampler;

	ComPtr<ID3D11SamplerState> pointClampSampler;

	ComPtr<ID3D11SamplerState> pointWrapSampler;

	//SRC_ALPHA INV_SRC_ALPHA
	ComPtr<ID3D11BlendState> defBlendState;

	//ONE ONE ALPHA ONE ZERO
	ComPtr<ID3D11BlendState> addtiveBlend;

	ComPtr<ID3D11RasterizerState> rasterCullBack;

	ComPtr<ID3D11RasterizerState> rasterCullFront;

	ComPtr<ID3D11RasterizerState> rasterCullNone;

	//体素化很有用 8xMSAA + Conservative Rasterization
	ComPtr<ID3D11RasterizerState2> rasterConserve;

	//depth less equal stencil disabled
	ComPtr<ID3D11DepthStencilState> defDepthStencilState;

private:

	friend class Aurora;

	static States* instance;

	States();
};

#endif // !_STATES_H_
