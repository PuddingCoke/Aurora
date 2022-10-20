#pragma once

#ifndef _RSVIEW_H_
#define _RSVIEW_H_

#include"RenderTargetView.h"
#include"ShaderResourceView.h"

class RSView :public RenderTargetView, public ShaderResourceView
{
public:

	RSView();

	virtual ~RSView();

	virtual void bindRTV() override;

	virtual void bindSRV() override;

};

#endif // !_RSVIEW_H_
