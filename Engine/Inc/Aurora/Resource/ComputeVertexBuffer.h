#pragma once

#ifndef _COMPUTEVERTEXBUFFER_H_
#define _COMPUTEVERTEXBUFFER_H_

#include<Aurora/Resource/VertexBuffer.h>
#include<Aurora/Core/DX/View/UnorderedAccessView.h>

class ComputeVertexBuffer :public VertexBuffer, public UnorderedAccessView
{
public:

	ComputeVertexBuffer(const UINT& byteWidth, const void* const data = nullptr);

	virtual ~ComputeVertexBuffer();

	virtual void bindVertexBuffer() override;

	virtual void bindCUAV() override;

	virtual void bindPUAV() override;

};

#endif // !_COMPUTEVERTEXBUFFER_H_
