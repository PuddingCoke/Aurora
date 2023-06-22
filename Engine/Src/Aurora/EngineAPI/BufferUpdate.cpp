#include<Aurora/EngineAPI/BufferUpdate.h>

BufferUpdate* BufferUpdate::instance = nullptr;

void BufferUpdate::pushBufferUpdateParam(Buffer* const buffer, const void* const dataPtr, const size_t& size)
{
	std::lock_guard<std::mutex> guard(instance->updateMutex);

	instance->params.push_back(BufferUpdateParam{ buffer,dataPtr,size });
}



void BufferUpdate::updateBuffer()
{
	std::lock_guard<std::mutex> guard(instance->updateMutex);

	D3D11_MAPPED_SUBRESOURCE mappedData = {};

	for (UINT i = 0; i < instance->params.size(); i++)
	{
		instance->ctx->Map(instance->params[i].buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		memcpy(mappedData.pData, instance->params[i].dataPtr, instance->params[i].size);

		instance->ctx->Unmap(instance->params[i].buffer, 0);
	}

	instance->params.clear();
}

void BufferUpdate::executeCommandList()
{
	std::lock_guard<std::mutex> guard(instance->updateMutex);

	ID3D11CommandList* list;

	instance->ctx->FinishCommandList(&list);

	ImCtx::get()->ExecuteCommandList(list);

	list->Release();
}

BufferUpdate::BufferUpdate()
{
	instance = this;

	ctx = new GraphicsContext();
}

BufferUpdate::~BufferUpdate()
{
	delete ctx;
}
