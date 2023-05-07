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

	virtual void bindCUAV() override;

	virtual void bindPUAV() override;

	virtual void bindSRV() override;

};

#endif // !_USVIEW_H_
