#include<Aurora/Resource/VertexBuffer.h>

VertexBuffer* VertexBuffer::curBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

ID3D11Buffer* const VertexBuffer::nullBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

const UINT VertexBuffer::nullStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

VertexBuffer::VertexBuffer(const UINT& byteWidth, const D3D11_USAGE& usage, const void* const data, const UINT& extraBindFlags, const UINT& miscFlags) :
	Buffer(byteWidth, D3D11_BIND_VERTEX_BUFFER | extraBindFlags, usage, data, usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0, miscFlags, 0),
	IASlot(-1)
{
}

VertexBuffer::~VertexBuffer()
{
}

bool VertexBuffer::unbindFromVertexBuffer(ID3D11DeviceContext3* const ctx)
{
	if (IASlot != -1)
	{
		ctx->IASetVertexBuffers(IASlot, 1, nullBuffer, nullStrides, nullStrides);
		curBuffer[IASlot] = nullptr;
		IASlot = -1;
		return true;
	}

	return false;
}

void VertexBuffer::bindVertexBuffer(ID3D11DeviceContext3* const ctx)
{
}
