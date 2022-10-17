#include<Aurora/DX/View/RSView.h>

RSView::RSView()
{

}

RSView::~RSView()
{
}

void RSView::bindRTV()
{
	unbindFromSRV();
}

void RSView::bindSRV()
{
	unbindFromRTV();
}
