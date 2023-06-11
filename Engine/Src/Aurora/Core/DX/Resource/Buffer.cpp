#include<Aurora/Core/DX/Resource/Buffer.h>

ID3D11Buffer* Buffer::getBuffer() const
{
	return buffer.Get();
}

void Buffer::updateSubresource(const void* const data) const
{
	Renderer::getContext()->UpdateSubresource(buffer.Get(), 0, nullptr, data, 0, 0);
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

		Renderer::get()->createBuffer(&bd, &subresource, buffer.ReleaseAndGetAddressOf());
	}
	else
	{
		Renderer::get()->createBuffer(&bd, nullptr, buffer.ReleaseAndGetAddressOf());
	}
}

Buffer::~Buffer()
{
}

D3D11_MAPPED_SUBRESOURCE Buffer::map(const D3D11_MAP& mapType, const unsigned int& mapFlags) const
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::getContext()->Map(buffer.Get(), 0, mapType, mapFlags, &mappedData);
	return mappedData;
}

void Buffer::unmap() const
{
	Renderer::getContext()->Unmap(buffer.Get(), 0);
}

ID3D11Resource* Buffer::getResource() const
{
	return buffer.Get();
}