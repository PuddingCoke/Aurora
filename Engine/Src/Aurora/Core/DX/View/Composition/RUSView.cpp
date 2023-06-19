#include<Aurora/Core/DX/View/Composition/RUSView.h>

RUSView::RUSView()
{
}

RUSView::~RUSView()
{
}

void RUSView::bindRTV(ID3D11DeviceContext3* const ctx)
{
	unbindFromSRV(ctx) || unbindFromCUAV(ctx) || unbindFromPUAV(ctx);
}

void RUSView::bindCUAV(ID3D11DeviceContext3* const ctx)
{
	unbindFromSRV(ctx) || unbindFromPUAV(ctx) || unbindFromRTV(ctx);
}

void RUSView::bindPUAV(ID3D11DeviceContext3* const ctx)
{
	unbindFromSRV(ctx) || unbindFromCUAV(ctx) || unbindFromRTV(ctx);
}

void RUSView::bindSRV(ID3D11DeviceContext3* const ctx)
{
	unbindFromCUAV(ctx) || unbindFromPUAV(ctx) || unbindFromRTV(ctx);
}
