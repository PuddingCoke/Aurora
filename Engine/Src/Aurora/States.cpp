#include<Aurora/States.h>

States* States::instance = nullptr;

ID3D11SamplerState* States::linearClampSampler;
ID3D11SamplerState* States::linearWrapSampler;
ID3D11SamplerState* States::pointClampSampler;
ID3D11SamplerState* States::pointWrapSampler;
ID3D11BlendState* States::defBlendState;
ID3D11BlendState* States::addtiveBlend;
ID3D11RasterizerState* States::rasterShadow;
ID3D11RasterizerState* States::rasterCullBack;
ID3D11RasterizerState* States::rasterCullFront;
ID3D11RasterizerState* States::rasterCullNone;
ID3D11RasterizerState2* States::rasterConserve;
ID3D11DepthStencilState* States::defDepthStencilState;
ID3D11DepthStencilState* States::depthStencilDisable;

States* States::get()
{
	return instance;
}

States::States()
{
	//初始化linearClampSampler
	{
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		RenderAPI::get()->CreateSamplerState(sampDesc, &linearClampSampler);
	}

	//初始化linearWrapSampler
	{
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		RenderAPI::get()->CreateSamplerState(sampDesc, &linearWrapSampler);
	}

	//初始化pointClampSampler
	{
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		RenderAPI::get()->CreateSamplerState(sampDesc, &pointClampSampler);
	}

	//初始化pointWrapSampler
	{
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		RenderAPI::get()->CreateSamplerState(sampDesc, &pointWrapSampler);
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

		RenderAPI::get()->CreateBlendState(blendStateDesc, &defBlendState);
	}

	//初始化addtiveBlend
	{
		D3D11_BLEND_DESC blendStateDesc = {};

		blendStateDesc.IndependentBlendEnable = false;

		blendStateDesc.RenderTarget[0].BlendEnable = true;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		RenderAPI::get()->CreateBlendState(blendStateDesc, &addtiveBlend);
	}

	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthBias = 16;
		rasterizerDesc.SlopeScaledDepthBias = 4.f;

		RenderAPI::get()->CreateRasterizerState(rasterizerDesc, &rasterShadow);
	}

	//初始化rasterCullBack
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;

		RenderAPI::get()->CreateRasterizerState(rasterizerDesc, &rasterCullBack);
	}

	//初始化rasterCullFront
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;

		RenderAPI::get()->CreateRasterizerState(rasterizerDesc, &rasterCullFront);
	}

	//初始化rasterCullNone
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		
		RenderAPI::get()->CreateRasterizerState(rasterizerDesc, &rasterCullNone);
	}

	//初始化rasterConserve
	{
		D3D11_RASTERIZER_DESC2 rasterizerDesc = {};
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.ForcedSampleCount = 8;
		rasterizerDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_ON;

		RenderAPI::get()->CreateRasterizerState2(rasterizerDesc, &rasterConserve);
	}

	//初始化defDepthStencilState
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		dsDesc.StencilEnable = false;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

		RenderAPI::get()->CreateDepthStencilState(dsDesc, &defDepthStencilState);
	}

	//初始化depthStencilDisable
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = false;
		dsDesc.StencilEnable = false;

		RenderAPI::get()->CreateDepthStencilState(dsDesc, &depthStencilDisable);
	}
}

States::~States()
{
	linearClampSampler->Release();
	linearWrapSampler->Release();
	pointClampSampler->Release();
	pointWrapSampler->Release();
	defBlendState->Release();
	addtiveBlend->Release();
	rasterShadow->Release();
	rasterCullBack->Release();
	rasterCullFront->Release();
	rasterCullNone->Release();
	rasterConserve->Release();
	defDepthStencilState->Release();
	depthStencilDisable->Release();
}
