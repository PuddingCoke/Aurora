#pragma once

#ifndef _STRUCTUREDBUFFER_H_
#define _STRUCTUREDBUFFER_H_

#include"../DX/Resource/Buffer.h"
#include"../DX/View/ShaderResourceView.h"

class StructuredBuffer :public Buffer, public ShaderResourceView
{
public:

	StructuredBuffer(const UINT& byteWidth, const UINT& structureByteStride, const D3D11_USAGE& usage, const void* const data = nullptr, const UINT& CPUAccessFlags = 0);

	virtual void bindSRV() override;

};

#endif // !_STRUCTUREDBUFFER_H_
