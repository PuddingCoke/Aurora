#include<Aurora/Resource/ComputeBuffer.h>

ComputeBuffer::ComputeBuffer(const UINT& byteWidth, const FMT& fmt, const void* const data) :
	Buffer(byteWidth, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT, data, 0, 0, 0)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = FMTCAST(fmt);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = byteWidth / FMTBYTESIZE(fmt);

		createSRV(buffer.Get(), srvDesc);
	}

	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = FMTCAST(fmt);
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = byteWidth / FMTBYTESIZE(fmt);
		uavDesc.Buffer.Flags = 0;

		createUAV(buffer.Get(), uavDesc);
	}
}

ComputeBuffer::~ComputeBuffer()
{
}
