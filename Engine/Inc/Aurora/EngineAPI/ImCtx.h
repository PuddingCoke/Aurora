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

	D3D11_MAPPED_SUBRESOURCE Map(Resource* const res, const UINT& subresource, const D3D11_MAP& mapType, const UINT& mapFlag = 0);

	void ExecuteCommandList(ID3D11CommandList* const list);

private:

	friend class Aurora;

	static ImCtx* instance;

	ImCtx();

	~ImCtx();

};

#endif // !_IMCTX_H_
