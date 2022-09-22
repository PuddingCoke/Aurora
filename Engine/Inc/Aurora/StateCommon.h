#pragma once

#ifndef _STATE_COMMON_H_
#define _STATE_COMMON_H_

#include<iostream>

#include"Renderer.h"

class StateCommon
{
public:

	StateCommon() = delete;

	StateCommon(const StateCommon&) = delete;

	void operator=(const StateCommon&) = delete;

	static ComPtr<ID3D11SamplerState> linearClampSampler;

	static ComPtr<ID3D11SamplerState> linearWrapSampler;

	static ComPtr<ID3D11SamplerState> pointClampSampler;

	static ComPtr<ID3D11SamplerState> pointWrapSampler;

	//SRC_ALPHA INV_SRC_ALPHA
	static ComPtr<ID3D11BlendState> defBlendState;

	//ONE ONE ALPHA ONE ZERO
	static ComPtr<ID3D11BlendState> addtiveBlend;

	static ComPtr<ID3D11RasterizerState> rasterCullBack;

	static ComPtr<ID3D11RasterizerState> rasterCullFront;

	static ComPtr<ID3D11RasterizerState> rasterCullNone;

	//depth less equal stencil disabled
	static ComPtr<ID3D11DepthStencilState> defDepthStencilState;

private:

	friend class Aurora;

	static HRESULT ini();

};

#endif // !_STATE_COMMON_H_
