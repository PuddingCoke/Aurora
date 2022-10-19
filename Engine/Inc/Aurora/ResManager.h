#pragma once

#ifndef _RESMANAGER_H_
#define _RESMANAGER_H_

#include"DX/View/RenderTargetView.h"
#include"DX/View/UnorderedAccessView.h"
#include"DX/View/ShaderResourceView.h"

#include"DX/Resource/Buffer.h"

/// <summary>
/// ֮ǰдOpenGL��ʱ��һ����Դ��Ӧ�ò����ͬʱ����Ϊ������Դ�������Դ��������d3d11ʱ��������Ϊ�ǲ�����ġ���֪��Effect 11���Խ��������⣬�����һ������Լ�дһ����Դ����㡣
/// </summary>
class ResManager
{
private:

	friend class Aurora;

	friend class RenderAPI;

	ResManager();

	static ResManager* get();

	void OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, ID3D11DepthStencilView* const dsv);

	void VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const unsigned int& slot);

	void VSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void HSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void DSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void GSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void PSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void CSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void IASetVertexBuffer(const unsigned int& slot, const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets);

	static ResManager* instance;

	ID3D11RenderTargetView* tempRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	ID3D11UnorderedAccessView* tempUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	ID3D11ShaderResourceView* tempSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ID3D11Buffer* tempBuffer[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	UINT tempStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT], tempOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

};

#endif // !_RESMANAGER_H_
