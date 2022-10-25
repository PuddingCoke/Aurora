#include<Aurora/DX/View/RUSView.h>

RUSView::RUSView()
{
}

RUSView::~RUSView()
{
}

void RUSView::bindRTV()
{
	unbindFromSRV() || unbindFromCUAV() || unbindFromPUAV();
}

void RUSView::bindCUAV()
{
	unbindFromSRV() || unbindFromPUAV() || unbindFromRTV();
}

void RUSView::bindPUAV()
{
	unbindFromSRV() || unbindFromCUAV() || unbindFromRTV();
}

void RUSView::bindSRV()
{
	unbindFromCUAV() || unbindFromPUAV() || unbindFromRTV();
}
