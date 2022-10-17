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

	ID3D11Buffer* get() const;

	D3D11_MAPPED_SUBRESOURCE map(const unsigned int& subresource, const D3D11_MAP& mapType = D3D11_MAP_WRITE_DISCARD, const unsigned int& mapFlags = 0) const;

	void unmap(const unsigned int& subresource) const;

protected:

	friend class ResManager;

	void VSSetBuffer(const unsigned int& slot = 0) const;

	void HSSetBuffer(const unsigned int& slot = 0) const;

	void DSSetBuffer(const unsigned int& slot = 0) const;
	
	void GSSetBuffer(const unsigned int& slot = 0) const;
	
	void PSSetBuffer(const unsigned int& slot = 0) const;
	
	void CSSetBuffer(const unsigned int& slot = 0) const;

	ComPtr<ID3D11Buffer> buffer;

};

#endif // !_BUFFER_H_
