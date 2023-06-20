#include<Aurora/Core/DX/View/Composition/RSView.h>

RSView::RSView()
{

}

RSView::~RSView()
{
}

void RSView::bindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromSRV(ctx, states);
}

void RSView::bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	unbindFromRTV(ctx, states);
}
