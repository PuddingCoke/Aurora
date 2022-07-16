#include<Aurora/StateCommon.h>

ComPtr<ID3D11SamplerState> StateCommon::defSamplerState;

ComPtr<ID3D11BlendState> StateCommon::defBlendState;

ComPtr<ID3D11BlendState> StateCommon::blendReplace;

HRESULT StateCommon::ini()
{
	//初始化defSamplerState
	{
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.BorderColor[0] = 0.f;
		sampDesc.BorderColor[1] = 0.f;
		sampDesc.BorderColor[2] = 0.f;
		sampDesc.BorderColor[3] = 0.f;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		Graphics::device->CreateSamplerState(&sampDesc, defSamplerState.ReleaseAndGetAddressOf());
	}

	//初始化defBlendState
	{
		D3D11_BLEND_DESC blendStateDesc = {};

		blendStateDesc.IndependentBlendEnable = false;

		blendStateDesc.RenderTarget[0].BlendEnable = true;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		Graphics::device->CreateBlendState(&blendStateDesc, defBlendState.ReleaseAndGetAddressOf());

		Graphics::setBlendState(defBlendState.Get());

	}

	//初始化blendReplace
	{
		D3D11_BLEND_DESC blendStateDesc = {};

		blendStateDesc.IndependentBlendEnable = false;

		blendStateDesc.RenderTarget[0].BlendEnable = true;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;

		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		Graphics::device->CreateBlendState(&blendStateDesc, blendReplace.ReleaseAndGetAddressOf());
	}

	return S_OK;
}
