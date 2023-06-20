#pragma once

#ifndef _SHADERRESOURCEVIEW_H_
#define _SHADERRESOURCEVIEW_H_

#include<Aurora/Core/GraphicsDevice.h>

class ShaderResourceView
{
public:

	ShaderResourceView();

	virtual ~ShaderResourceView();

	ShaderResourceView(const ShaderResourceView&) = delete;

	void operator=(const ShaderResourceView&) = delete;

	ID3D11ShaderResourceView* getSRV() const;

	ID3D11ShaderResourceView** releaseAndGetSRV();

	void createSRV(ID3D11Resource* const resource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc);

	//解决binding hazard的问题
	virtual void bindSRV(ID3D11DeviceContext3* const ctx) = 0;

	//返回是否成功解绑
	bool unbindFromSRV(ID3D11DeviceContext3* const ctx);

private:

	friend class ImCtx;

	static ShaderResourceView* curVSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curHSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curDSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curGSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curPSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curCSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ID3D11ShaderResourceView* const nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

	void unbindVSRV(ID3D11DeviceContext3* const ctx);

	void unbindHSRV(ID3D11DeviceContext3* const ctx);

	void unbindDSRV(ID3D11DeviceContext3* const ctx);

	void unbindGSRV(ID3D11DeviceContext3* const ctx);

	void unbindPSRV(ID3D11DeviceContext3* const ctx);

	void unbindCSRV(ID3D11DeviceContext3* const ctx);

	ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	int VSSlot;

	int HSSlot;

	int DSSlot;

	int GSSlot;

	int PSSlot;

	int CSSlot;

};

#endif // !_SHADERRESOURCEVIEW_H_
