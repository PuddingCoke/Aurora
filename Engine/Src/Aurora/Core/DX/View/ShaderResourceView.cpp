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
	unbindFromSRV(Renderer::getContext());
}

ID3D11ShaderResourceView* ShaderResourceView::getSRV() const
{
	return shaderResourceView.Get();
}

ID3D11ShaderResourceView** ShaderResourceView::releaseAndGetSRV()
{
	return shaderResourceView.ReleaseAndGetAddressOf();
}

void ShaderResourceView::unbindVSRV(ID3D11DeviceContext3* const ctx)
{
	ctx->VSSetShaderResources(VSSlot, 1, nullSRV);
	curVSRV[VSSlot] = nullptr;
	VSSlot = -1;
}

void ShaderResourceView::unbindHSRV(ID3D11DeviceContext3* const ctx)
{
	ctx->HSSetShaderResources(HSSlot, 1, nullSRV);
	curHSRV[HSSlot] = nullptr;
	HSSlot = -1;
}

void ShaderResourceView::unbindDSRV(ID3D11DeviceContext3* const ctx)
{
	ctx->DSSetShaderResources(DSSlot, 1, nullSRV);
	curDSRV[DSSlot] = nullptr;
	DSSlot = -1;
}

void ShaderResourceView::unbindGSRV(ID3D11DeviceContext3* const ctx)
{
	ctx->GSSetShaderResources(GSSlot, 1, nullSRV);
	curGSRV[GSSlot] = nullptr;
	GSSlot = -1;
}

void ShaderResourceView::unbindPSRV(ID3D11DeviceContext3* const ctx)
{
	ctx->PSSetShaderResources(PSSlot, 1, nullSRV);
	curPSRV[PSSlot] = nullptr;
	PSSlot = -1;
}

void ShaderResourceView::unbindCSRV(ID3D11DeviceContext3* const ctx)
{
	ctx->CSSetShaderResources(CSSlot, 1, nullSRV);
	curCSRV[CSSlot] = nullptr;
	CSSlot = -1;
}

bool ShaderResourceView::unbindFromSRV(ID3D11DeviceContext3* const ctx)
{
	bool success = false;

	if (VSSlot != -1)
	{
		unbindVSRV(ctx);
		success = true;
	}
	if (HSSlot != -1)
	{
		unbindHSRV(ctx);
		success = true;
	}
	if (DSSlot != -1)
	{
		unbindDSRV(ctx);
		success = true;
	}
	if (GSSlot != -1)
	{
		unbindGSRV(ctx);
		success = true;
	}
	if (PSSlot != -1)
	{
		unbindPSRV(ctx);
		success = true;
	}
	if (CSSlot != -1)
	{
		unbindCSRV(ctx);
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
