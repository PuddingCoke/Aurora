#pragma once

#ifndef _RESOURCEBUFFER_H_
#define _RESOURCEBUFFER_H_

#include<Aurora/Core/DX/Resource/Buffer.h>
#include<Aurora/Core/DX/View/ShaderResourceView.h>

class ResourceBuffer :public Buffer, public ShaderResourceView
{
public:

	ResourceBuffer() = delete;

	ResourceBuffer(const ResourceBuffer&) = delete;

	void operator=(const ResourceBuffer&) = delete;

	ResourceBuffer(const UINT& byteWidth, const D3D11_USAGE& usage, const FMT& srvFmt,const void* const data = nullptr);

	~ResourceBuffer();

	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

};

#endif // !_RESOURCEBUFFER_H_
