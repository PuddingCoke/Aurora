#pragma once

#ifndef _STRUCTUREDBUFFER_H_
#define _STRUCTUREDBUFFER_H_

#include<Aurora/Core/DX/Resource/Buffer.h>
#include<Aurora/Core/DX/View/ShaderResourceView.h>

class StructuredBuffer :public Buffer, public ShaderResourceView
{
public:

	StructuredBuffer(const UINT& byteWidth, const UINT& structureByteStride, const D3D11_USAGE& usage, const void* const data = nullptr);

	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

};

#endif // !_STRUCTUREDBUFFER_H_
