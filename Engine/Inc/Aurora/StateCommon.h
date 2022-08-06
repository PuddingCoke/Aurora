#pragma once

#ifndef _STATE_COMMON_H_
#define _STATE_COMMON_H_

#include<iostream>

#include"Graphics.h"

class StateCommon
{
public:

	static ComPtr<ID3D11SamplerState> defSamplerState;

	//SRC_ALPHA INV_SRC_ALPHA
	static ComPtr<ID3D11BlendState> defBlendState;

	//ONE ZERO
	static ComPtr<ID3D11BlendState> blendReplace;

	//ONE ONE ALPHA ONE ZERO
	static ComPtr<ID3D11BlendState> addtiveBlend;

	static ComPtr<ID3D11DepthStencilState> defDepthStencilState;

private:

	friend class Aurora;

	static HRESULT ini();

};

#endif // !_STATE_COMMON_H_
