#pragma once

#ifndef _UNORDEREDACCESSVIEW_H_
#define _UNORDEREDACCESSVIEW_H_

#include<Aurora/Core/Renderer.h>

class UnorderedAccessView
{
public:

	UnorderedAccessView();

	virtual ~UnorderedAccessView();

	UnorderedAccessView(const UnorderedAccessView&) = delete;

	void operator=(const UnorderedAccessView&) = delete;

	ID3D11UnorderedAccessView* getUAV() const;

	ID3D11UnorderedAccessView** releaseAndGetUAV();

	void createUAV(ID3D11Resource* const resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc);

	//解决binding hazard的问题
	virtual void bindCUAV(ID3D11DeviceContext3* const ctx) = 0;

	virtual void bindPUAV(ID3D11DeviceContext3* const ctx) = 0;

	static void unbindCUAV(ID3D11DeviceContext3* const ctx);

	static void unbindPUAV(ID3D11DeviceContext3* const ctx);

	//是否成功解绑
	bool unbindFromCUAV(ID3D11DeviceContext3* const ctx);

	bool unbindFromPUAV(ID3D11DeviceContext3* const ctx);

private:

	friend class ImCtx;

	static UnorderedAccessView* curCUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	static UnorderedAccessView* curPUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	static ID3D11UnorderedAccessView* const nullUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;

	int CUAVSlot;

	bool boundOnRTV;

};

#endif // !_UNORDEREDACCESSVIEW_H_

