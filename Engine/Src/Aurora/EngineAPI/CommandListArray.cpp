#include<Aurora/EngineAPI/CommandListArray.h>

CommandListArray* CommandListArray::instance = nullptr;

void CommandListArray::pushCommandList(ID3D11CommandList* const commandList)
{
	instance->commandListArray.push(commandList);
}

void CommandListArray::executeCommandLists()
{
	while (!instance->commandListArray.empty())
	{
		ID3D11CommandList* list = instance->commandListArray.front();
		instance->commandListArray.pop();
		ImCtx::get()->ExecuteCommandList(list);
		list->Release();
	}
}

CommandListArray::CommandListArray()
{
	instance = this;
}

CommandListArray::~CommandListArray()
{
}
