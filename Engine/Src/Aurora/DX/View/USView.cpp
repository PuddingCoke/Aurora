#include<Aurora/DX/View/USView.h>

USView::USView()
{
}

USView::~USView()
{
}

void USView::bindUAV()
{
	unbindFromSRV();
}

void USView::bindSRV()
{
	unbindFromUAV();
}
