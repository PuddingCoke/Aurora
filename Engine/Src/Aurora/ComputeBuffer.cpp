#include<Aurora/ComputeBuffer.h>

ComputeBuffer::ComputeBuffer(const UINT& byteWidth, const UINT& extraBindFlags, const void* const data, const UINT& cpuaccessFlags, const UINT& miscFlags, const UINT& structureByteStride) :
	Buffer(byteWidth, D3D11_BIND_UNORDERED_ACCESS | extraBindFlags, D3D11_USAGE_DEFAULT, data, cpuaccessFlags, miscFlags, structureByteStride)
{
}

ComputeBuffer::~ComputeBuffer()
{
}
