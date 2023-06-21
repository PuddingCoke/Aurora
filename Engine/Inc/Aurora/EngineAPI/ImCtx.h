#pragma once

#ifndef _IMCTX_H_
#define _IMCTX_H_

#include<Aurora/Core/GraphicsContext.h>

class ImCtx :public GraphicsContext
{
public:

	static ImCtx* get();

	ImCtx(const ImCtx&) = delete;

	void operator=(const ImCtx&) = delete;

private:

	friend class Aurora;

	static ImCtx* instance;

	ImCtx();

	~ImCtx();

};

#endif // !_IMCTX_H_
