#include<Aurora/Core/DX/Resource/Buffer.h>

Buffer* Buffer::curBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

ID3D11Buffer* Buffer::nullBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

UINT Buffer::nullStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

ID3D11Buffer* Buffer::getBuffer() const
{
	return buffer.Get();
}

void Buffer::updateSubresource(const void* const data) const
{
	Renderer::getContext()->UpdateSubresource(buffer.Get(), 0, nullptr, data, 0, 0);
}

Buffer::Buffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage, const void* const data, const UINT& CPUAccessFlags, const UINT& miscFlags, const UINT& structureByteStride) :
	IASlot(-1), startConstants(0), numConstants((byteWidth / 16 + 15) & ~15)
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
		Renderer::getDevice()->CreateBuffer(&bd, &subresource, buffer.ReleaseAndGetAddressOf());
	}
	else
	{
		Renderer::getDevice()->CreateBuffer(&bd, nullptr, buffer.ReleaseAndGetAddressOf());
	}
}

Buffer::~Buffer()
{
	unbindFromVertexBuffer();
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

Buffer::Buffer()
{
}

bool Buffer::unbindFromVertexBuffer()
{
	if (IASlot != -1)
	{
		Renderer::getContext()->IASetVertexBuffers(IASlot, 1, nullBuffer, nullStrides, nullStrides);
		curBuffer[IASlot] = nullptr;
		IASlot = -1;
		return true;
	}

	return false;
}

void Buffer::bindVertexBuffer()
{
}