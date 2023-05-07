#pragma once

#ifndef _DEPTHSTENCILVIEW_H_
#define _DEPTHSTENCILVIEW_H_

#include<Aurora/Core/Renderer.h>

class DepthStencilView
{
public:

	DepthStencilView();

	DepthStencilView(ID3D11Resource* const resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);

	virtual ~DepthStencilView();

	DepthStencilView(const DepthStencilView&) = delete;

	void operator=(const DepthStencilView&) = delete;

	void clearDSV(const UINT& clearFlag, const float& depth = 1.0f, const UINT8& stencil = 0) const;

	ID3D11DepthStencilView* getDSV() const;

	virtual void bindDSV();

protected:

	void createDSV(ID3D11Resource* const resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);

	ComPtr<ID3D11DepthStencilView> depthStencilView;

};

#endif // !_DEPTHSTENCILVIEW_H_
