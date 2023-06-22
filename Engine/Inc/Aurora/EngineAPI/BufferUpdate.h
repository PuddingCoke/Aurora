#pragma once

#include<d3d11_4.h>
#include<mutex>
#include<vector>

#include"ImCtx.h"

class BufferUpdate
{
public:

	static void pushBufferUpdateParam(Buffer* const buffer, const void* const dataPtr, const size_t& size);

private:

	friend class Aurora;

	BufferUpdate();

	~BufferUpdate();

	static BufferUpdate* get();

	static BufferUpdate* instance;

	static void updateBuffer();

	static void executeCommandList();

	struct BufferUpdateParam
	{
		Buffer* const buffer;
		const void* const dataPtr;
		const size_t size;
	};

	std::mutex updateMutex;

	std::vector<BufferUpdateParam> params;

	GraphicsContext* ctx;

};