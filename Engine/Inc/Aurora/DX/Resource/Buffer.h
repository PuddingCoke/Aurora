#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include<iostream>
#include<Aurora/Renderer.h>

class Buffer
{
public:

	Buffer() = delete;

	Buffer(const Buffer&) = delete;

	void operator=(const Buffer&) = delete;

	Buffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage,
		const void* const data = nullptr, const UINT& cpuaccessFlags = 0, const UINT& miscFlags = 0, const UINT& structureByteStride = 0);

	ID3D11Buffer* getBuffer() const;

	D3D11_MAPPED_SUBRESOURCE map(const unsigned int& subresource, const D3D11_MAP& mapType = D3D11_MAP_WRITE_DISCARD, const unsigned int& mapFlags = 0) const;

	void unmap(const unsigned int& subresource) const;

protected:

	bool unbindFromVertexBuffer();

private:

	friend class ResManager;

	static Buffer* curBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	static ID3D11Buffer* nullBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	static UINT nullStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	static void unbindVertexBuffer();

	//解决绑定时的binding hazard
	virtual void bindVertexBuffer();

	ComPtr<ID3D11Buffer> buffer;

	bool boundOnIA;

};

#endif // !_BUFFER_H_
