#include<Aurora/DX/ShaderResourceView.h>

ShaderResourceView::ShaderResourceView()
{
}

ShaderResourceView::~ShaderResourceView()
{
}

void ShaderResourceView::VSSetSRV(const unsigned int& slot) const
{
	Renderer::context->VSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
}

void ShaderResourceView::GSSetSRV(const unsigned int& slot) const
{
	Renderer::context->GSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
}

void ShaderResourceView::PSSetSRV(const unsigned int& slot) const
{
	Renderer::context->PSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
}

void ShaderResourceView::CSSetSRV(const unsigned int& slot) const
{
	Renderer::context->CSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
}

void ShaderResourceView::DSSetSRV(const unsigned int& slot) const
{
	Renderer::context->DSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
}

void ShaderResourceView::HSSetSRV(const unsigned int& slot) const
{
	Renderer::context->HSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
}

ID3D11ShaderResourceView* ShaderResourceView::getSRV() const
{
	return shaderResourceView.Get();
}
