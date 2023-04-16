#include<Aurora/StructuredBuffer.h>

StructuredBuffer::StructuredBuffer(const UINT& byteWidth, const UINT& structureByteStride, const D3D11_USAGE& usage, const void* const data, const UINT& CPUAccessFlags) :
	Buffer(byteWidth, D3D11_BIND_SHADER_RESOURCE, usage, data, CPUAccessFlags, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, structureByteStride)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = byteWidth / structureByteStride;

	createSRV(getBuffer(), srvDesc);
}

void StructuredBuffer::bindSRV()
{

}