#include<Aurora/DX/Resource/Buffer.h>

Buffer* Buffer::curBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

ID3D11Buffer* Buffer::nullBuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

UINT Buffer::nullStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

ID3D11Buffer* Buffer::getBuffer() const
{
	return buffer.Get();
}

Buffer::Buffer(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage, const void* const data, const UINT& cpuaccessFlags, const UINT& miscFlags, const UINT& structureByteStride) :
	boundOnIA(false)
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

D3D11_MAPPED_SUBRESOURCE Buffer::map(const unsigned int& subresource, const D3D11_MAP& mapType, const unsigned int& mapFlags) const
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(buffer.Get(), subresource, mapType, mapFlags, &mappedData);
	return mappedData;
}

void Buffer::unmap(const unsigned int& subresource) const
{
	Renderer::context->Unmap(buffer.Get(), subresource);
}

bool Buffer::unbindFromVertexBuffer()
{
	if (boundOnIA)
	{
		unbindVertexBuffer();
		return true;
	}

	return false;
}

void Buffer::unbindVertexBuffer()
{
	for (unsigned int i = 0; curBuffer[i]; i++)
	{
		curBuffer[i]->boundOnIA = false;
		curBuffer[i] = nullptr;
	}
	Renderer::context->IASetVertexBuffers(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, nullBuffer, nullStrides, nullStrides);
}

void Buffer::bindVertexBuffer()
{
}