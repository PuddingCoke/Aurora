#pragma once

#ifndef _COMMANDLISTARRAY_H_
#define _COMMANDLISTARRAY_H_

#include<d3d11_4.h>
#include<queue>

#include"ImCtx.h"

class CommandListArray
{
public:

	static void pushCommandList(ID3D11CommandList* const commandList);

private:

	friend class Aurora;

	static CommandListArray* instance;

	static void executeCommandLists();

	CommandListArray();

	~CommandListArray();

	std::queue<ID3D11CommandList*> commandListArray;

};

#endif // !_COMMANDLISTARRAY_H_
