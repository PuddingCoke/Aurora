#pragma once

#ifndef _USVIEW_H_
#define _USVIEW_H_

#include"../UnorderedAccessView.h"
#include"../ShaderResourceView.h"

class USView : public UnorderedAccessView, public ShaderResourceView
{
public:

	USView();

	virtual ~USView();

	virtual void bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	virtual void bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

};

#endif // !_USVIEW_H_
