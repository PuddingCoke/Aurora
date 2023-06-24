#pragma once

#ifndef _COMPUTESTRUCTUREDBUFFER_H_
#define _COMPUTESTRUCTUREDBUFFER_H_

#include<Aurora/Core/DX/Resource/Buffer.h>

#include<Aurora/Core/DX/View/Composition/USView.h>

class ComputeStructuredBuffer :public Buffer, public USView
{
public:

	ComputeStructuredBuffer(const UINT& byteWidth, const UINT& structureByteStride, const void* const data = nullptr);

	~ComputeStructuredBuffer();

};

#endif // !_COMPUTESTRUCTUREDBUFFER_H_
