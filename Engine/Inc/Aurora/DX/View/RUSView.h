#pragma once

#ifndef _RUSVIEW_H_
#define _RUSVIEW_H_

#include"RenderTargetView.h"
#include"UnorderedAccessView.h"
#include"ShaderResourceView.h"

class RUSView :public RenderTargetView, public UnorderedAccessView, public ShaderResourceView
{
public:

	RUSView();

	virtual ~RUSView();

	virtual void bindRTV() override;

	virtual void bindUAV() override;

	virtual void bindSRV() override;

};

#endif // !_RUSVIEW_H_