#include<Aurora/Buffer.h>

ID3D11Buffer* Buffer::constantBuffers[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

Buffer* Buffer::create(const UINT& byteWidth, const UINT& bindFlags, const D3D11_USAGE& usage, const void* const data, const UINT& cpuaccessFlags, const UINT& miscFlags, const UINT& structureByteStride)
{
	return new Buffer(byteWidth, bindFlags, usage, data, cpuaccessFlags, miscFlags, structureByteStride);
}

ID3D11Buffer* Buffer::get() const
{
	return buffer.Get();
}

void Buffer::VSSetBuffer(const unsigned int& slot)
{
	Renderer::context->VSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::GSSetBuffer(const unsigned int& slot)
{
	Renderer::context->GSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::PSSetBuffer(const unsigned int& slot)
{
	Renderer::context->PSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::CSSetBuffer(const unsigned int& slot)
{
	Renderer::context->CSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::DSSetBuffer(const unsigned int& slot)
{
	Renderer::context->DSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::HSSetBuffer(const unsigned int& slot)
{
	Renderer::context->HSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

void Buffer::VSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();
	for (int i = 0; i < buffers.size(); i++)
	{
		constantBuffers[i] = it[0]->buffer.Get();
		++it;
	}
	Renderer::context->VSSetConstantBuffers(slot, (unsigned int)buffers.size(), constantBuffers);
}

void Buffer::GSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();
	for (int i = 0; i < buffers.size(); i++)
	{
		constantBuffers[i] = it[0]->buffer.Get();
		++it;
	}
	Renderer::context->GSSetConstantBuffers(slot, (unsigned int)buffers.size(), constantBuffers);
}

void Buffer::PSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();
	for (int i = 0; i < buffers.size(); i++)
	{
		constantBuffers[i] = it[0]->buffer.Get();
		++it;
	}
	Renderer::context->PSSetConstantBuffers(slot, (unsigned int)buffers.size(), constantBuffers);
}

void Buffer::CSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();
	for (int i = 0; i < buffers.size(); i++)
	{
		constantBuffers[i] = it[0]->buffer.Get();
		++it;
	}
	Renderer::context->CSSetConstantBuffers(slot, (unsigned int)buffers.size(), constantBuffers);
}

void Buffer::DSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();
	for (int i = 0; i < buffers.size(); i++)
	{
		constantBuffers[i] = it[0]->buffer.Get();
		++it;
	}
	Renderer::context->DSSetConstantBuffers(slot, (unsigned int)buffers.size(), constantBuffers);
}

void Buffer::HSSetBuffers(std::initializer_list<Buffer*> buffers, const unsigned int& slot)
{
	std::initializer_list<Buffer*>::iterator it = buffers.begin();
	for (int i = 0; i < buffers.size(); i++)
	{
		constantBuffers[i] = it[0]->buffer.Get();
		++it;
	}
	Renderer::context->HSSetConstantBuffers(slot, (unsigned int)buffers.size(), constantBuffers);
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
