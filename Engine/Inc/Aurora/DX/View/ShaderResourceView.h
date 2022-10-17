#pragma once

#ifndef _SHADERRESOURCEVIEW_H_
#define _SHADERRESOURCEVIEW_H_

#include<Aurora/Renderer.h>

class ShaderResourceView
{
public:

	ShaderResourceView();

	virtual ~ShaderResourceView();

	ShaderResourceView(const ShaderResourceView&) = delete;

	void operator=(const ShaderResourceView&) = delete;

	ID3D11ShaderResourceView* getSRV() const;

protected:

	//�����Ƿ�ɹ����
	bool unbindFromSRV();

	ComPtr<ID3D11ShaderResourceView> shaderResourceView;

private:

	friend class ResManager;

	static ShaderResourceView* curVSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curHSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curDSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curGSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curPSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ShaderResourceView* curCSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	static ID3D11ShaderResourceView* const nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

	void unbindVSRV();

	void unbindHSRV();

	void unbindDSRV();

	void unbindGSRV();

	void unbindPSRV();

	void unbindCSRV();

	//���binding hazard������
	virtual void bindSRV();

	int VSSlot;

	int HSSlot;

	int DSSlot;

	int GSSlot;

	int PSSlot;

	int CSSlot;

};

#endif // !_SHADERRESOURCEVIEW_H_
