#include<Aurora/Core/DX/View/Composition/RSView.h>

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
