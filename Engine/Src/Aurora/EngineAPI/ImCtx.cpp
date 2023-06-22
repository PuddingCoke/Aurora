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

D3D11_MAPPED_SUBRESOURCE ImCtx::Map(Resource* const res, const UINT& subresource, const D3D11_MAP& mapType, const UINT& mapFlag)
{
	D3D11_MAPPED_SUBRESOURCE data = {};

	getContext()->Map(res->getResource(), subresource, mapType, mapFlag, &data);

	return data;
}

void ImCtx::ExecuteCommandList(ID3D11CommandList* const list)
{
	getContext()->ExecuteCommandList(list, false);
}

ImCtx::~ImCtx()
{
	delete defRenderTargetView;
}