#include<Aurora/DBuffer.h>

DBuffer* DBuffer::create(const UINT& byteWidth, const UINT& bindFlags, const void* const data, const UINT& miscFlags, const UINT& structureByteStride)
{
	return new DBuffer(byteWidth, bindFlags, data, miscFlags, structureByteStride);
}

D3D11_MAPPED_SUBRESOURCE DBuffer::map(const unsigned int& subresource, const D3D11_MAP& mapType, const unsigned int& mapFlags)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	return mappedData;
}

void DBuffer::ummap(const unsigned int& subresource)
{
	Renderer::context->Unmap(buffer.Get(), 0);
}

DBuffer::DBuffer(const UINT& byteWidth, const UINT& bindFlags, const void* const data, const UINT& miscFlags, const UINT& structureByteStride) :
	Buffer(byteWidth, bindFlags, D3D11_USAGE_DYNAMIC, data, D3D11_CPU_ACCESS_WRITE, miscFlags, structureByteStride)
{
}
