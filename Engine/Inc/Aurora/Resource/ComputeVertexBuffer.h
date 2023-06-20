#pragma once

#ifndef _COMPUTEVERTEXBUFFER_H_
#define _COMPUTEVERTEXBUFFER_H_

#include<Aurora/Resource/VertexBuffer.h>
#include<Aurora/Core/DX/View/UnorderedAccessView.h>

class ComputeVertexBuffer :public VertexBuffer, public UnorderedAccessView
{
public:

	ComputeVertexBuffer(const UINT& byteWidth, const FMT& fmt, const void* const data = nullptr);

	virtual ~ComputeVertexBuffer();

	virtual void bindVertexBuffer(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	virtual void bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	virtual void bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

};

#endif // !_COMPUTEVERTEXBUFFER_H_
