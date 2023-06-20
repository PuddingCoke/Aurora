#pragma once

#ifndef _RENDERTARGETVIEW_H_
#define _RENDERTARGETVIEW_H_

#include<Aurora/Core/GraphicsDevice.h>

#include<Aurora/Core/GraphicsStates.h>

struct GraphicsStates;

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

	//解决binding hazard的问题
	virtual void bindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) = 0;

	static void unbindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

	//是否成功解绑
	bool unbindFromRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

private:

	friend class ImCtx;

	ComPtr<ID3D11RenderTargetView> renderTargetView;

	bool boundOnRTV;
};

#endif // !_RENDERTARGETVIEW_H_
