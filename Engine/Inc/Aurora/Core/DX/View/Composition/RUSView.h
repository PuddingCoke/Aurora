#pragma once

#ifndef _RUSVIEW_H_
#define _RUSVIEW_H_

#include"../RenderTargetView.h"
#include"../UnorderedAccessView.h"
#include"../ShaderResourceView.h"

class RUSView :public RenderTargetView, public UnorderedAccessView, public ShaderResourceView
{
public:

	RUSView();

	virtual ~RUSView();

	virtual void bindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	virtual void bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	virtual void bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

};

#endif // !_RUSVIEW_H_