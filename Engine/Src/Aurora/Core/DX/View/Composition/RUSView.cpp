#include<Aurora/Core/DX/View/Composition/RUSView.h>

RUSView::RUSView()
{
}

RUSView::~RUSView()
{
}

void RUSView::bindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromSRV(ctx, states) || unbindFromCUAV(ctx, states) || unbindFromPUAV(ctx, states);
}

void RUSView::bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromSRV(ctx, states) || unbindFromPUAV(ctx, states) || unbindFromRTV(ctx, states);
}

void RUSView::bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromSRV(ctx, states) || unbindFromCUAV(ctx, states) || unbindFromRTV(ctx, states);
}

void RUSView::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromCUAV(ctx, states) || unbindFromPUAV(ctx, states) || unbindFromRTV(ctx, states);
}
