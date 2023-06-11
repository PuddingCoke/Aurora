#include<Aurora/Resource/ComputeBuffer.h>

ComputeBuffer::ComputeBuffer(const UINT& byteWidth, const FMT& fmt, const void* const data) :
	Buffer(byteWidth, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT, data, 0, 0, 0)
{
}

ComputeBuffer::~ComputeBuffer()
{
}
