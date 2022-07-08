#pragma once

#ifndef _STATE_COMMON_H_
#define _STATE_COMMON_H_

#include<iostream>

#include"Graphics.h"

class StateCommon
{
public:

	static ComPtr<ID3D11SamplerState> defSamplerState;

private:

	friend class Aurora;

	static HRESULT ini();

};

#endif // !_STATE_COMMON_H_
