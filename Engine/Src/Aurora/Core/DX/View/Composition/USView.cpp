#include<Aurora/Core/DX/View/Composition/USView.h>

USView::USView()
{
}

USView::~USView()
{
}

void USView::bindCUAV(ID3D11DeviceContext3* const ctx)
{
	unbindFromPUAV(ctx) || unbindFromSRV(ctx);
}

void USView::bindPUAV(ID3D11DeviceContext3* const ctx)
{
	unbindFromCUAV(ctx) || unbindFromSRV(ctx);
}

void USView::bindSRV(ID3D11DeviceContext3* const ctx)
{
	unbindFromCUAV(ctx) || unbindFromPUAV(ctx);
}
