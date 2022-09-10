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

private:

	Buffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage,
		const void* const data, const UINT& cpuaccessFlags, const UINT& miscFlags, const UINT& structureByteStride);

	ComPtr<ID3D11Buffer> buffer;

};

#endif // !_BUFFER_H_
