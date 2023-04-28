#include<Aurora/PerframeCB.h>

ComPtr<ID3D11Buffer> PerframeCB::globalBuffer;

void* PerframeCB::pData = nullptr;

UINT PerframeCB::curStartConstant = 0;

PerframeCB::PerframeCB(const UINT& byteWidth)
{
	globalBuffer.CopyTo(buffer.ReleaseAndGetAddressOf());

	IASlot = -1;

	startConstants = 0;

	numConstants = (byteWidth / 16 + 15) & ~15;
}

void PerframeCB::mapPerframeBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;

	Renderer::getContext()->Map(globalBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	pData = mappedData.pData;
}

void PerframeCB::updateData(PerframeCB* const perframeCB, const void* const data, const size_t& size)
{
	perframeCB->startConstants = curStartConstant;

	void* writeLoc = (void*)((char*)pData + curStartConstant * 16);

	memcpy(writeLoc, data, size);

	curStartConstant += (UINT)((size / 16 + 15) & ~15);
}

void PerframeCB::unmapPerframeBuffer()
{
	curStartConstant = 0;

	Renderer::getContext()->Unmap(globalBuffer.Get(), 0);
}

void PerframeCB::ini()
{
	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = 1024 * 1024 * 4;//4 megabytes
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Renderer::getDevice()->CreateBuffer(&bd, nullptr, globalBuffer.ReleaseAndGetAddressOf());
	}
}

void PerframeCB::release()
{
	globalBuffer.Reset();
}
