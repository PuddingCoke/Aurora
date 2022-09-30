#include<Aurora/IBuffer.h>

IBuffer* IBuffer::create(const UINT& byteWidth, const UINT& bindFlags, const void* const data, const UINT& miscFlags, const UINT& structureByteStride)
{
	return new IBuffer(byteWidth, bindFlags, data, miscFlags, structureByteStride);
}

IBuffer::IBuffer(const UINT& byteWidth, const UINT& bindFlags, const void* const data, const UINT& miscFlags, const UINT& structureByteStride) :
	Buffer(byteWidth, bindFlags, D3D11_USAGE_IMMUTABLE, data, 0, miscFlags, structureByteStride)
{
}
