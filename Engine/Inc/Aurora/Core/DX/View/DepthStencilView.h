#pragma once

#ifndef _DEPTHSTENCILVIEW_H_
#define _DEPTHSTENCILVIEW_H_

#include<Aurora/Core/GraphicsDevice.h>

class DepthStencilView
{
public:

	DepthStencilView();

	DepthStencilView(ID3D11Resource* const resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);

	virtual ~DepthStencilView();

	DepthStencilView(const DepthStencilView&) = delete;

	void operator=(const DepthStencilView&) = delete;

	ID3D11DepthStencilView* getDSV() const;

	virtual void bindDSV(ID3D11DeviceContext3* const ctx);

protected:

	void createDSV(ID3D11Resource* const resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);

	ComPtr<ID3D11DepthStencilView> depthStencilView;

};

#endif // !_DEPTHSTENCILVIEW_H_
