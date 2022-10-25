#pragma once

#ifndef _COMPUTEBUFFER_H_
#define _COMPUTEBUFFER_H_

#include"DX/Resource/Buffer.h"
#include"DX/View/UnorderedAccessView.h"

class ComputeBuffer :public Buffer, public UnorderedAccessView
{
public:

	ComputeBuffer(const D3D11_UNORDERED_ACCESS_VIEW_DESC& uavDesc, const UINT& byteWidth, const UINT& extraBindFlags = 0, const void* const data = nullptr, const UINT& cpuaccessFlags = 0, const UINT& miscFlags = 0, const UINT& structureByteStride = 0);

	virtual ~ComputeBuffer();

	virtual void bindVertexBuffer() override;

	virtual void bindCUAV() override;

	virtual void bindPUAV() override;

};

#endif // !_COMPUTEBUFFER_H_
