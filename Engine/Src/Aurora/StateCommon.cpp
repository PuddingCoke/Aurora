#include<Aurora/StateCommon.h>

ComPtr<ID3D11SamplerState> StateCommon::defSamplerState;

HRESULT StateCommon::ini()
{
	//³õÊ¼»¯defSamplerState
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

	return S_OK;
}
