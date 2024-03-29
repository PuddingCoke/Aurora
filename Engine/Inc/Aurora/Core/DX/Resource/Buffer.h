#pragma once

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include"Resource.h"

class Buffer :public Resource
{
public:

	virtual ~Buffer();

	Buffer() = delete;

	Buffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage,
		const void* const data, const UINT& CPUAccessFlags, const UINT& miscFlags, const UINT& structureByteStride);

	ID3D11Resource* getResource() const override;

	ID3D11Buffer* getBuffer() const;

protected:

	ComPtr<ID3D11Buffer> buffer;

};

#endif // !_BUFFER_H_
