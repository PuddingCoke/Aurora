#pragma once

#ifndef _RENDERTARGETVIEW_H_
#define _RENDERTARGETVIEW_H_

#include<Aurora/Renderer.h>

class RenderTargetView
{
public:

	RenderTargetView();

	virtual ~RenderTargetView();

	RenderTargetView(const RenderTargetView&) = delete;

	void operator=(const RenderTargetView&) = delete;

	void clearRTV(const float* color);

protected:

	//是否成功解绑
	bool unbindFromRTV();

	ComPtr<ID3D11RenderTargetView> renderTargetView;

private:

	friend class ResManager;

	static RenderTargetView* curRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	static ID3D11RenderTargetView* const nullRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	static void unbindRTV();

	//解决binding hazard的问题
	virtual void bindRTV() = 0;

	bool boundOnRTV;
};

#endif // !_RENDERTARGETVIEW_H_
