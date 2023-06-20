#include<Aurora/Core/DX/Resource/Buffer.h>

ID3D11Buffer* Buffer::getBuffer() const
{
	return buffer.Get();
}

Buffer::Buffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage, const void* const data, const UINT& CPUAccessFlags, const UINT& miscFlags, const UINT& structureByteStride)
{
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = byteWidth;
	bd.BindFlags = bindFlags;
	bd.Usage = usage;
	bd.CPUAccessFlags = CPUAccessFlags;
	bd.MiscFlags = miscFlags;
	bd.StructureByteStride = structureByteStride;

	if (data)
	{
		D3D11_SUBRESOURCE_DATA subresource = {};
		subresource.pSysMem = data;

		GraphicsDevice::get()->createBuffer(&bd, &subresource, buffer.ReleaseAndGetAddressOf());
	}
	else
	{
		GraphicsDevice::get()->createBuffer(&bd, nullptr, buffer.ReleaseAndGetAddressOf());
	}
}

Buffer::~Buffer()
{
}

ID3D11Resource* Buffer::getResource() const
{
	return buffer.Get();
}