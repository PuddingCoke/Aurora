#include<Aurora/Resource/ConstantBuffer.h>

ConstantBuffer::ConstantBuffer(const UINT& byteWidth, const D3D11_USAGE& usage, const void* const data, const UINT& miscFlags) :
	Buffer(byteWidth, D3D11_BIND_CONSTANT_BUFFER, usage, data, usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0, miscFlags)
{
}
