#include<Aurora/EngineAPI/ImCtx.h>

ImCtx* ImCtx::instance = nullptr;

ImCtx::ImCtx()
{
	instance = this;
}

ImCtx* ImCtx::get()
{
	return instance;
}

ImCtx::~ImCtx()
{
	delete defRenderTargetView;
}