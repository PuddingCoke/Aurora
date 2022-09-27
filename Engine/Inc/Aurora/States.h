#pragma once

#ifndef _STATES_H_
#define _STATES_H_

#include<iostream>

#include"Renderer.h"

//这里定义了一些有用的状态
class States
{
public:

	States() = delete;

	States(const States&) = delete;

	void operator=(const States&) = delete;

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

#endif // !_STATES_H_
