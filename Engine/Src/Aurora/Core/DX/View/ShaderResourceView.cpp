#include<Aurora/Core/DX/View/ShaderResourceView.h>

ShaderResourceView* ShaderResourceView::curVSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT] = {};

ShaderResourceView* ShaderResourceView::curHSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT] = {};

ShaderResourceView* ShaderResourceView::curDSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT] = {};

ShaderResourceView* ShaderResourceView::curGSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT] = {};

ShaderResourceView* ShaderResourceView::curPSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT] = {};

ShaderResourceView* ShaderResourceView::curCSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT] = {};

ID3D11ShaderResourceView* const ShaderResourceView::nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT] = {};

ShaderResourceView::ShaderResourceView() :
	VSSlot(-1), HSSlot(-1), DSSlot(-1), GSSlot(-1), PSSlot(-1), CSSlot(-1)
{
}

ShaderResourceView::~ShaderResourceView()
{
	unbindFromSRV();
}

ID3D11ShaderResourceView* ShaderResourceView::getSRV() const
{
	return shaderResourceView.Get();
}

ID3D11ShaderResourceView** ShaderResourceView::releaseAndGetSRV()
{
	return shaderResourceView.ReleaseAndGetAddressOf();
}

void ShaderResourceView::unbindVSRV()
{
	Renderer::getContext()->VSSetShaderResources(VSSlot, 1, nullSRV);
	curVSRV[VSSlot] = nullptr;
	VSSlot = -1;
}

void ShaderResourceView::unbindHSRV()
{
	Renderer::getContext()->HSSetShaderResources(HSSlot, 1, nullSRV);
	curHSRV[HSSlot] = nullptr;
	HSSlot = -1;
}

void ShaderResourceView::unbindDSRV()
{
	Renderer::getContext()->DSSetShaderResources(DSSlot, 1, nullSRV);
	curDSRV[DSSlot] = nullptr;
	DSSlot = -1;
}

void ShaderResourceView::unbindGSRV()
{
	Renderer::getContext()->GSSetShaderResources(GSSlot, 1, nullSRV);
	curGSRV[GSSlot] = nullptr;
	GSSlot = -1;
}

void ShaderResourceView::unbindPSRV()
{
	Renderer::getContext()->PSSetShaderResources(PSSlot, 1, nullSRV);
	curPSRV[PSSlot] = nullptr;
	PSSlot = -1;
}

void ShaderResourceView::unbindCSRV()
{
	Renderer::getContext()->CSSetShaderResources(CSSlot, 1, nullSRV);
	curCSRV[CSSlot] = nullptr;
	CSSlot = -1;
}

bool ShaderResourceView::unbindFromSRV()
{
	bool success = false;

	if (VSSlot != -1)
	{
		unbindVSRV();
		success = true;
	}
	if (HSSlot != -1)
	{
		unbindHSRV();
		success = true;
	}
	if (DSSlot != -1)
	{
		unbindDSRV();
		success = true;
	}
	if (GSSlot != -1)
	{
		unbindGSRV();
		success = true;
	}
	if (PSSlot != -1)
	{
		unbindPSRV();
		success = true;
	}
	if (CSSlot != -1)
	{
		unbindCSRV();
		success = true;
	}

	return success;
}

void ShaderResourceView::createSRV(ID3D11Resource* const resource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc)
{
	Renderer::get()->createShaderResourceView(resource, &desc, shaderResourceView.ReleaseAndGetAddressOf());
}

void ShaderResourceView::generateMips() const
{
	Renderer::getContext()->GenerateMips(shaderResourceView.Get());
}
