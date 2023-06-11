#pragma once

#ifndef _COMPUTEBUFFER_H_
#define _COMPUTEBUFFER_H_

#include<Aurora/Core/DX/Resource/Buffer.h>
#include<Aurora/Core/DX/View/Composition/USView.h>

class ComputeBuffer :public Buffer, public USView
{
public:

	ComputeBuffer(const UINT& byteWidth, const FMT& fmt, const void* const data);

	virtual ~ComputeBuffer();

};

#endif // !_COMPUTEBUFFER_H_
