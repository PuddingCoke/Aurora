#pragma once

#ifndef _RENDERTARGETVIEW_H_
#define _RENDERTARGETVIEW_H_

#include<Aurora/Core/GraphicsDevice.h>

class RenderTargetView
{
public:

	RenderTargetView();

	RenderTargetView(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc);

	virtual ~RenderTargetView();

	RenderTargetView(const RenderTargetView&) = delete;

	void operator=(const RenderTargetView&) = delete;


	ID3D11RenderTargetView* getRTV() const;

	ID3D11RenderTargetView** releaseAndGetRTV();

	void createRTV(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc);

	//���binding hazard������
	virtual void bindRTV(ID3D11DeviceContext3* const ctx) = 0;

	static void unbindRTV(ID3D11DeviceContext3* const ctx);

	//�Ƿ�ɹ����
	bool unbindFromRTV(ID3D11DeviceContext3* const ctx);

private:

	friend class ImCtx;

	static RenderTargetView* curRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	ComPtr<ID3D11RenderTargetView> renderTargetView;

	bool boundOnRTV;
};

#endif // !_RENDERTARGETVIEW_H_
