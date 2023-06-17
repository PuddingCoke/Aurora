#include<Aurora/Core/DX/View/Special/RenderOnlyRTV.h>

RenderOnlyRTV::RenderOnlyRTV(ID3D11Resource* resource, const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc)
{
	createRTV(resource, rtvDesc);
}

void RenderOnlyRTV::bindRTV()
{
}
