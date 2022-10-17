#include<Aurora/DX/Resource/Buffer.h>

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

void Buffer::VSSetBuffer(const unsigned int& slot) const
{
	Renderer::context->VSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::HSSetBuffer(const unsigned int& slot) const
{
	Renderer::context->HSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::DSSetBuffer(const unsigned int& slot) const
{
	Renderer::context->DSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::GSSetBuffer(const unsigned int& slot) const
{
	Renderer::context->GSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::PSSetBuffer(const unsigned int& slot) const
{
	Renderer::context->PSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::CSSetBuffer(const unsigned int& slot) const
{
	Renderer::context->CSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}