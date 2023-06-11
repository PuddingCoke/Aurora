#include<Aurora/Resource/ComputeVertexBuffer.h>

ComputeVertexBuffer::ComputeVertexBuffer(const UINT& byteWidth, const FMT& fmt, const void* const data) :
	VertexBuffer(byteWidth, D3D11_USAGE_DEFAULT, data, D3D11_BIND_UNORDERED_ACCESS, 0)
{
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = FMTCAST(fmt);
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = 0;
		uavDesc.Buffer.NumElements = byteWidth / FMTBYTESIZE(fmt);

		createUAV(getBuffer(), uavDesc);
	}
}

ComputeVertexBuffer::~ComputeVertexBuffer()
{
	unbindFromCUAV() || unbindFromPUAV() || unbindFromVertexBuffer();
}

void ComputeVertexBuffer::bindVertexBuffer()
{
	unbindFromCUAV() || unbindFromPUAV();
}

void ComputeVertexBuffer::bindCUAV()
{
	unbindFromVertexBuffer();
}

void ComputeVertexBuffer::bindPUAV()
{
	unbindFromVertexBuffer();
}
