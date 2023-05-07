#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include"Resource.h"

class Buffer:public Resource
{
public:

	Buffer(const Buffer&) = delete;

	void operator=(const Buffer&) = delete;

	Buffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage,
		const void* const data = nullptr, const UINT& CPUAccessFlags = 0, const UINT& miscFlags = 0, const UINT& structureByteStride = 0);

	virtual ~Buffer();

	void updateSubresource(const void* const data, const unsigned int& subresource) const;

	D3D11_MAPPED_SUBRESOURCE map(const D3D11_MAP& mapType = D3D11_MAP_WRITE_DISCARD, const unsigned int& mapFlags = 0) const;

	void unmap() const;

	ID3D11Resource* getResource() const override;

	ID3D11Buffer* getBuffer() const;

protected:

	Buffer();

	bool unbindFromVertexBuffer();

	ComPtr<ID3D11Buffer> buffer;

	int IASlot;

	UINT startConstants;

	UINT numConstants;

private:

	friend class RenderAPI;

	static Buffer* curBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	static ID3D11Buffer* nullBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	static UINT nullStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	//解决绑定时的binding hazard
	virtual void bindVertexBuffer();

};

#define BUFFERDYN(STRUCT) Buffer(sizeof(STRUCT),D3D11_BIND_CONSTANT_BUFFER,D3D11_USAGE_DYNAMIC,nullptr,D3D11_CPU_ACCESS_WRITE,0,0)

#define BUFFERIMU(STRUCT) Buffer(sizeof(STRUCT),D3D11_BIND_CONSTANT_BUFFER,D3D11_USAGE_IMMUTABLE,&STRUCT,0,0,0)

#endif // !_BUFFER_H_
