#pragma once

#ifndef _VERTEXBUFFER_H_
#define _VERTEXBUFFER_H_

#include<Aurora/Core/DX/Resource/Buffer.h>

#include<Aurora/Core/GraphicsStates.h>

struct GraphicsStates;

class VertexBuffer :public Buffer
{
public:

	VertexBuffer(const VertexBuffer&) = delete;

	void operator=(const VertexBuffer&) = delete;

	VertexBuffer(const UINT& byteWidth, const D3D11_USAGE& usage, const void* const data = nullptr, const UINT& extraBindFlags = 0, const UINT& miscFlags = 0);

	virtual ~VertexBuffer();

protected:

	bool unbindFromVertexBuffer(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

private:

	friend class GraphicsContext;

	int IASlot;

	static ID3D11Buffer* const nullBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	static const UINT nullStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	//解决绑定时的binding hazard
	virtual void bindVertexBuffer(ID3D11DeviceContext3* const ctx, GraphicsStates* const states);

};

#endif // !_VERTEXBUFFER_H_
