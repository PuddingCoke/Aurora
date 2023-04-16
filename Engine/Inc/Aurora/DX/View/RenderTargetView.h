#pragma once

#ifndef _RENDERTARGETVIEW_H_
#define _RENDERTARGETVIEW_H_

#include<Aurora/Renderer.h>

class RenderTargetView
{
public:

	RenderTargetView();

	RenderTargetView(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc);

	virtual ~RenderTargetView();

	RenderTargetView(const RenderTargetView&) = delete;

	void operator=(const RenderTargetView&) = delete;

	void clearRTV(const float* color);

	ID3D11RenderTargetView* getRTV() const;

	ID3D11RenderTargetView** releaseAndGetRTV();

	void createRTV(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& desc);

	//���binding hazard������
	virtual void bindRTV() = 0;

	static void unbindRTV();

	//�Ƿ�ɹ����
	bool unbindFromRTV();

private:

	friend class ResManager;

	static RenderTargetView* curRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	ComPtr<ID3D11RenderTargetView> renderTargetView;

	bool boundOnRTV;
};

#endif // !_RENDERTARGETVIEW_H_
