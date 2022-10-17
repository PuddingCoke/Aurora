#pragma once

#ifndef _RESMANAGER_H_
#define _RESMANAGER_H_

#include"DX/View/RenderTargetView.h"
#include"DX/View/UnorderedAccessView.h"
#include"DX/View/ShaderResourceView.h"

#include"DX/Resource/Buffer.h"

class ResManager
{
public:

	static ResManager* get();

	void OMSetRTV(std::initializer_list<RenderTargetView*> rtvs, ID3D11DepthStencilView* const dsv);

	void VSSetSRV(std::initializer_list<ShaderResourceView*> srvs, const unsigned int& slot);

	void DSSetSRV(std::initializer_list<ShaderResourceView*> srvs, const unsigned int& slot);

	void HSSetSRV(std::initializer_list<ShaderResourceView*> srvs, const unsigned int& slot);

	void GSSetSRV(std::initializer_list<ShaderResourceView*> srvs, const unsigned int& slot);

	void PSSetSRV(std::initializer_list<ShaderResourceView*> srvs, const unsigned int& slot);

	void CSSetSRV(std::initializer_list<ShaderResourceView*> srvs, const unsigned int& slot);

	void CSSetUAV(std::initializer_list<UnorderedAccessView*> uavs, const unsigned int& slot);

	void VSSetBuffer(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	void HSSetBuffer(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	void DSSetBuffer(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	void GSSetBuffer(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	void PSSetBuffer(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	void CSSetBuffer(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

private:

	ResManager();

	friend class Aurora;

	static ResManager* instance;

	ID3D11RenderTargetView* tempRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	ID3D11UnorderedAccessView* tempUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	ID3D11ShaderResourceView* tempSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ID3D11Buffer* tempBuffer[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

};

#endif // !_RESMANAGER_H_
