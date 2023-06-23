#include<Aurora/Core/DX/View/ShaderResourceView.h>

ID3D11ShaderResourceView* const ShaderResourceView::nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT] = {};

ShaderResourceView::ShaderResourceView() :
	VSSlot(-1), HSSlot(-1), DSSlot(-1), GSSlot(-1), PSSlot(-1), CSSlot(-1)
{
}

ShaderResourceView::~ShaderResourceView()
{
}

ID3D11ShaderResourceView* ShaderResourceView::getSRV() const
{
	return shaderResourceView.Get();
}

ID3D11ShaderResourceView** ShaderResourceView::releaseAndGetSRV()
{
	return shaderResourceView.ReleaseAndGetAddressOf();
}

void ShaderResourceView::unbindVSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	ctx->VSSetShaderResources(VSSlot, 1, nullSRV);
	states->curVSRV[VSSlot] = nullptr;
	VSSlot = -1;
}

void ShaderResourceView::unbindHSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	ctx->HSSetShaderResources(HSSlot, 1, nullSRV);
	states->curHSRV[HSSlot] = nullptr;
	HSSlot = -1;
}

void ShaderResourceView::unbindDSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	ctx->DSSetShaderResources(DSSlot, 1, nullSRV);
	states->curDSRV[DSSlot] = nullptr;
	DSSlot = -1;
}

void ShaderResourceView::unbindGSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	ctx->GSSetShaderResources(GSSlot, 1, nullSRV);
	states->curGSRV[GSSlot] = nullptr;
	GSSlot = -1;
}

void ShaderResourceView::unbindPSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	ctx->PSSetShaderResources(PSSlot, 1, nullSRV);
	states->curPSRV[PSSlot] = nullptr;
	PSSlot = -1;
}

void ShaderResourceView::unbindCSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	ctx->CSSetShaderResources(CSSlot, 1, nullSRV);
	states->curCSRV[CSSlot] = nullptr;
	CSSlot = -1;
}

bool ShaderResourceView::unbindFromSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	bool success = false;

	if (VSSlot != -1)
	{
		unbindVSRV(ctx, states);
		success = true;
	}
	if (HSSlot != -1)
	{
		unbindHSRV(ctx, states);
		success = true;
	}
	if (DSSlot != -1)
	{
		unbindDSRV(ctx, states);
		success = true;
	}
	if (GSSlot != -1)
	{
		unbindGSRV(ctx, states);
		success = true;
	}
	if (PSSlot != -1)
	{
		unbindPSRV(ctx, states);
		success = true;
	}
	if (CSSlot != -1)
	{
		unbindCSRV(ctx, states);
		success = true;
	}

	if (success)
	{
		states->managedSRV.erase(std::remove(states->managedSRV.begin(), states->managedSRV.end(), this));
	}

	return success;
}

void ShaderResourceView::createSRV(ID3D11Resource* const resource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc)
{
	GraphicsDevice::get()->createShaderResourceView(resource, &desc, shaderResourceView.ReleaseAndGetAddressOf());
}

void ShaderResourceView::pushToManagedSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states)
{
	std::list<ShaderResourceView*>::iterator it = std::find(states->managedSRV.begin(), states->managedSRV.end(), this);

	if (it == states->managedSRV.end())
	{
		states->managedSRV.push_back(this);
	}
}
