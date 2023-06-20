#include<Aurora/Core/DX/View/Composition/USView.h>

USView::USView()
{
}

USView::~USView()
{
}

void USView::bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromPUAV(ctx,states) || unbindFromSRV(ctx,states);
}

void USView::bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromCUAV(ctx, states) || unbindFromSRV(ctx, states);
}

void USView::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromCUAV(ctx, states) || unbindFromPUAV(ctx, states);
}
