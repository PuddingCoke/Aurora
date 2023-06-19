#include<Aurora/Resource/ResourceBuffer.h>

ResourceBuffer::ResourceBuffer(const UINT& byteWidth, const D3D11_USAGE& usage, const FMT& srvFmt, const void* const data) :
	Buffer(byteWidth, D3D11_BIND_SHADER_RESOURCE, usage, data, usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0, 0, 0)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(srvFmt);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = byteWidth / FMTBYTESIZE(srvFmt);

		createSRV(buffer.Get(), srvDesc);
	}
}

ResourceBuffer::~ResourceBuffer()
{
}

void ResourceBuffer::bindSRV(ID3D11DeviceContext3* const ctx)
{
}
