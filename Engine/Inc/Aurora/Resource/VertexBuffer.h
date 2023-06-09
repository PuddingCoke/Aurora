#pragma once

#ifndef _VERTEXBUFFER_H_
#define _VERTEXBUFFER_H_

#include<Aurora/Core/DX/Resource/Buffer.h>

class VertexBuffer :public Buffer
{
public:

	VertexBuffer(const VertexBuffer&) = delete;

	void operator=(const VertexBuffer&) = delete;

	VertexBuffer(const UINT& byteWidth, const D3D11_USAGE& usage, const void* const data = nullptr, const UINT& extraBindFlags = 0, const UINT& miscFlags = 0);

	virtual ~VertexBuffer();

	int IASlot;

protected:

	bool unbindFromVertexBuffer();

private:

	friend class RenderAPI;

	static VertexBuffer* curBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	static ID3D11Buffer* const nullBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	static const UINT nullStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	//解决绑定时的binding hazard
	virtual void bindVertexBuffer();

};

#endif // !_VERTEXBUFFER_H_
