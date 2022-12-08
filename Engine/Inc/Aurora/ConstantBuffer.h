#pragma once

#ifndef _CONSTANTBUFFER_H_
#define _CONSTANTBUFFER_H_

#include"DX/Resource/Buffer.h"

class ConstantBuffer :public Buffer
{
public:

	ConstantBuffer() = delete;

	ConstantBuffer(const ConstantBuffer&) = delete;

	void operator=(const ConstantBuffer&) = delete;

	ConstantBuffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage,
		const void* const data = nullptr, const UINT& cpuaccessFlags = 0, const UINT& miscFlags = 0);

};

#endif // !_CONSTANTBUFFER_H_
