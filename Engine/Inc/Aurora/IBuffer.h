#pragma once

#ifndef _IBUFFER_H_
#define _IBUFFER_H_

#include"Buffer.h"

class IBuffer :public Buffer
{
public:

	static IBuffer* create(const UINT& byteWidth, const UINT& bindFlags, const void* const data, const UINT& miscFlags = 0, const UINT& structureByteStride = 0);

private:

	IBuffer(const UINT& byteWidth, const UINT& bindFlags, const void* const data, const UINT& miscFlags, const UINT& structureByteStride);

};

#endif // !_IBUFFER_H_
