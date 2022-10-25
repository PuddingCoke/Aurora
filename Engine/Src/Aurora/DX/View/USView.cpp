#include<Aurora/DX/View/USView.h>

USView::USView()
{
}

USView::~USView()
{
}

void USView::bindCUAV()
{
	unbindFromPUAV() || unbindFromSRV();
}

void USView::bindPUAV()
{
	unbindFromCUAV() || unbindFromSRV();
}

void USView::bindSRV()
{
	unbindFromCUAV() || unbindFromPUAV();
}
