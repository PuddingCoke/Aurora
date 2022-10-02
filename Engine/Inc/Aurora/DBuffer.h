#pragma once

#ifndef _DBUFFER_H_
#define _DBUFFER_H_

#include"Buffer.h"

class DBuffer :public Buffer
{
public:

	static DBuffer* create(const UINT& byteWidth, const UINT& bindFlags, const void* const data = nullptr, const UINT& miscFlags = 0, const UINT& structureByteStride = 0);

	D3D11_MAPPED_SUBRESOURCE map(const unsigned int& subresource, const D3D11_MAP& mapType = D3D11_MAP_WRITE_DISCARD, const unsigned int& mapFlags = 0);

	void unmap(const unsigned int& subresource);

private:

	DBuffer(const UINT& byteWidth, const UINT& bindFlags, const void* const data, const UINT& miscFlags, const UINT& structureByteStride);

};

#endif // !_DBUFFER_H_
