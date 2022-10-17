#pragma once

#ifndef _UNORDEREDACCESSVIEW_H_
#define _UNORDEREDACCESSVIEW_H_

#include<Aurora/Renderer.h>

class UnorderedAccessView
{
public:

	UnorderedAccessView();

	virtual ~UnorderedAccessView();

	UnorderedAccessView(const UnorderedAccessView&) = delete;

	void operator=(const UnorderedAccessView&) = delete;

	ID3D11UnorderedAccessView** ReleaseAndGetAddressOf();

protected:

	//是否成功解绑
	bool unbindFromUAV();

	ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;

private:

	friend class ResManager;

	static UnorderedAccessView* curUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	static ID3D11UnorderedAccessView* const nullUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	//解决binding hazard的问题
	virtual void bindUAV() = 0;

	int UAVSlot;

};

#endif // !_UNORDEREDACCESSVIEW_H_

