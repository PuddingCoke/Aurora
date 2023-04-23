#pragma once

#ifndef _RESMANAGER_H_
#define _RESMANAGER_H_

#include"DX/View/RenderTargetView.h"
#include"DX/View/UnorderedAccessView.h"
#include"DX/View/ShaderResourceView.h"
#include"DX/View/DepthStencilView.h"

#include"DX/Resource/Buffer.h"

/// <summary>
/// 之前写OpenGL的时候一个资源在应用层可以同时被绑定为输入资源和输出资源，当我用d3d11时，这种行为是不允许的。我知道Effect 11可以解决这个问题，但是我还是想自己写一个资源管理层。
/// </summary>
class ResManager
{
private:

	friend class Aurora;

	friend class RenderAPI;

	ResManager();

	ResManager(const ResManager&) = delete;

	void operator=(const ResManager&) = delete;

	static ResManager* get();

	void OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, DepthStencilView* const dsv);

	void OMSetUAV(const std::initializer_list<UnorderedAccessView*> uavs);

	void VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const unsigned int& slot);

	void VSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void HSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void DSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void GSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void PSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void CSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void IASetVertexBuffer(const unsigned int& slot, const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets);

	static ResManager* instance;

	ID3D11RenderTargetView* tempRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	ID3D11UnorderedAccessView* tempUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	ID3D11ShaderResourceView* tempSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ID3D11Buffer* tempBuffer[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	UINT tempStartConstants[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	UINT tempNumConstants[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	ID3D11SamplerState* tempSampler[D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT];

	UINT tempStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT], tempOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

};

#endif // !_RESMANAGER_H_
