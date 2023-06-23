#pragma once

#ifndef _SHADERRESOURCEVIEW_H_
#define _SHADERRESOURCEVIEW_H_

#include<Aurora/Core/GraphicsDevice.h>

#include<Aurora/Core/GraphicsStates.h>

struct GraphicsStates;

class ShaderResourceView
{
public:

	ShaderResourceView();

	virtual ~ShaderResourceView();

	ID3D11ShaderResourceView* getSRV() const;

	void createSRV(ID3D11Resource* const resource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc);

	//解决binding hazard的问题
	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) = 0;

	//返回是否成功解绑
	bool unbindFromSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

protected:

	friend class GraphicsContext;

	static ID3D11ShaderResourceView* const nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

	void unbindVSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

	void unbindHSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

	void unbindDSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

	void unbindGSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

	void unbindPSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

	void unbindCSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

	ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	int VSSlot;

	int HSSlot;

	int DSSlot;

	int GSSlot;

	int PSSlot;

	int CSSlot;

};

#endif // !_SHADERRESOURCEVIEW_H_
