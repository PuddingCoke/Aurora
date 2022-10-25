#include<Aurora/ComputeBuffer.h>

ComputeBuffer::ComputeBuffer(const D3D11_UNORDERED_ACCESS_VIEW_DESC& uavDesc, const UINT& byteWidth, const UINT& extraBindFlags, const void* const data, const UINT& cpuaccessFlags, const UINT& miscFlags, const UINT& structureByteStride) :
	Buffer(byteWidth, D3D11_BIND_UNORDERED_ACCESS | extraBindFlags, D3D11_USAGE_DEFAULT, data, cpuaccessFlags, miscFlags, structureByteStride)
{
	createUAV(getBuffer(), uavDesc);
}

ComputeBuffer::~ComputeBuffer()
{
}

void ComputeBuffer::bindVertexBuffer()
{
	unbindFromCUAV() || unbindFromPUAV();
}

void ComputeBuffer::bindCUAV()
{
	unbindFromVertexBuffer();
}

void ComputeBuffer::bindPUAV()
{
	unbindFromVertexBuffer();
}
