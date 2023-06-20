#pragma once

#ifndef _RSVIEW_H_
#define _RSVIEW_H_

#include"../RenderTargetView.h"
#include"../ShaderResourceView.h"

class RSView :public RenderTargetView, public ShaderResourceView
{
public:

	RSView();

	virtual ~RSView();

	virtual void bindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

};

#endif // !_RSVIEW_H_
