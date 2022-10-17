#include<Aurora/DX/View/RUSView.h>

RUSView::RUSView()
{
}

RUSView::~RUSView()
{
}

void RUSView::bindRTV()
{
	if (!unbindFromSRV())
	{
		unbindFromUAV();
	}
}

void RUSView::bindUAV()
{
	if (!unbindFromSRV())
	{
		unbindFromRTV();
	}
}

void RUSView::bindSRV()
{
	if (!unbindFromUAV())
	{
		unbindFromRTV();
	}
}
