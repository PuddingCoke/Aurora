#include<Aurora/ConstantBuffer.h>

ConstantBuffer::ConstantBuffer(const UINT& byteWidth, const D3D11_USAGE& usage, const void* const data, const UINT& CPUAccessFlags, const UINT& miscFlags) :
	Buffer(byteWidth, D3D11_BIND_CONSTANT_BUFFER, usage, data, CPUAccessFlags, miscFlags)
{
}
