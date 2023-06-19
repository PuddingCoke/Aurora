#pragma once

#ifndef _RENDERONLYRTV_H_
#define _RENDERONLYRTV_H_

#include<Aurora/Core/DX/View/RenderTargetView.h>

class RenderOnlyRTV :public RenderTargetView
{
public:

	RenderOnlyRTV(ID3D11Resource* resource, const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc);

	void bindRTV(ID3D11DeviceContext3* const ctx) override;

};

#endif // !_RENDERONLYRTV_H_
