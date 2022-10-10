#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include"Renderer.h"
#include<iostream>

class Buffer
{
public:

	Buffer() = delete;

	Buffer(const Buffer&) = delete;

	void operator=(const Buffer&) = delete;

	static Buffer* create(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage = D3D11_USAGE_DEFAULT,
		const void* const data = nullptr, const UINT& cpuaccessFlags = 0, const UINT& miscFlags = 0, const UINT& structureByteStride = 0);

	ID3D11Buffer* get() const;

	void VSSetBuffer(const unsigned int& slot = 0) const;

	void GSSetBuffer(const unsigned int& slot = 0) const;

	void PSSetBuffer(const unsigned int& slot = 0) const;

	void CSSetBuffer(const unsigned int& slot = 0) const;

	void DSSetBuffer(const unsigned int& slot = 0) const;

	void HSSetBuffer(const unsigned int& slot = 0) const;

	static void VSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	static void GSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	static void PSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	static void CSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	static void DSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

	static void HSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot);

private:

	static ID3D11Buffer* constantBuffers[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

protected:

	Buffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage,
		const void* const data, const UINT& cpuaccessFlags, const UINT& miscFlags, const UINT& structureByteStride);

	ComPtr<ID3D11Buffer> buffer;

};

#endif // !_BUFFER_H_
