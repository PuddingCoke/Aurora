#include<Aurora/Resource/ComputeStructuredBuffer.h>

ComputeStructuredBuffer::ComputeStructuredBuffer(const UINT& byteWidth, const UINT& structureByteStride, const void* const data) :
	Buffer(byteWidth, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT, data, 0, 0, structureByteStride)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = byteWidth / structureByteStride;

		createSRV(buffer.Get(), srvDesc);
	}

	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = byteWidth / structureByteStride;
		uavDesc.Buffer.Flags = 0;

		createUAV(buffer.Get(), uavDesc);
	}
}

ComputeStructuredBuffer::~ComputeStructuredBuffer()
{
}
