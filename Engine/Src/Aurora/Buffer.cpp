#include<Aurora/Buffer.h>

Buffer* Buffer::create(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage, const void* const data, const UINT& cpuaccessFlags, const UINT& miscFlags, const UINT& structureByteStride)
{
	return new Buffer(byteWidth, bindFlags, usage, data, cpuaccessFlags, miscFlags, structureByteStride);
}

ID3D11Buffer* Buffer::get() const
{
	return buffer.Get();
}

Buffer::Buffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage, const void* const data, const UINT& cpuaccessFlags, const UINT& miscFlags, const UINT& structureByteStride)
{
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = byteWidth;
	bd.BindFlags = bindFlags;
	bd.Usage = usage;
	bd.CPUAccessFlags = cpuaccessFlags;
	bd.MiscFlags = miscFlags;
	bd.StructureByteStride = structureByteStride;

	if (data)
	{
		D3D11_SUBRESOURCE_DATA subresource = {};
		subresource.pSysMem = data;
		Renderer::device->CreateBuffer(&bd, &subresource, buffer.ReleaseAndGetAddressOf());
	}
	else
	{
		Renderer::device->CreateBuffer(&bd, nullptr, buffer.ReleaseAndGetAddressOf());
	}
}
