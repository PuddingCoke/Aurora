#include<Aurora/States.h>

States* States::instance = nullptr;

ID3D11SamplerState* States::anisotropicClampSampler = nullptr;
ID3D11SamplerState* States::anisotropicWrapSampler = nullptr;
ID3D11SamplerState* States::linearClampSampler = nullptr;
ID3D11SamplerState* States::linearWrapSampler = nullptr;
ID3D11SamplerState* States::pointClampSampler = nullptr;
ID3D11SamplerState* States::pointWrapSampler = nullptr;
ID3D11SamplerState* States::shadowSampler = nullptr;
ID3D11BlendState* States::defBlendState = nullptr;
ID3D11BlendState* States::addtiveBlend = nullptr;
ID3D11RasterizerState* States::rasterShadow = nullptr;
ID3D11RasterizerState* States::rasterCullBack = nullptr;
ID3D11RasterizerState* States::rasterCullFront = nullptr;
ID3D11RasterizerState* States::rasterCullNone = nullptr;
ID3D11RasterizerState2* States::rasterConserve = nullptr;
ID3D11DepthStencilState* States::defDepthStencilState = nullptr;
ID3D11DepthStencilState* States::depthStencilDisable = nullptr;

States* States::get()
{
	return instance;
}

States::States()
{
	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MaxAnisotropy = 16;
		desc.MinLOD = 0.f;
		desc.MaxLOD = FLT_MAX;

		Renderer::device->CreateSamplerState(&desc, &anisotropicClampSampler);
	}

	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MaxAnisotropy = 16;
		desc.MinLOD = 0.f;
		desc.MaxLOD = FLT_MAX;

		Renderer::device->CreateSamplerState(&desc, &anisotropicWrapSampler);
	}

	//初始化linearClampSampler
	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		Renderer::device->CreateSamplerState(&desc, &linearClampSampler);
	}

	//初始化linearWrapSampler
	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		Renderer::device->CreateSamplerState(&desc, &linearWrapSampler);
	}

	//初始化pointClampSampler
	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		Renderer::device->CreateSamplerState(&desc, &pointClampSampler);
	}

	//初始化pointWrapSampler
	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		Renderer::device->CreateSamplerState(&desc, &pointWrapSampler);
	}

	{
		D3D11_SAMPLER_DESC desc = {};
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.f;
		desc.ComparisonFunc = D3D11_COMPARISON_GREATER;
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.MinLOD = 0.f;
		desc.MaxLOD = FLT_MAX;

		Renderer::device->CreateSamplerState(&desc, &shadowSampler);
	}

	//初始化defBlendState
	{
		D3D11_BLEND_DESC blendStateDesc = {};

		blendStateDesc.IndependentBlendEnable = false;

		blendStateDesc.RenderTarget[0].BlendEnable = true;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		Renderer::device->CreateBlendState(&blendStateDesc, &defBlendState);
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

		Renderer::device->CreateBlendState(&blendStateDesc, &addtiveBlend);
	}

	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.DepthBias = 16;
		rasterizerDesc.SlopeScaledDepthBias = 4.f;

		Renderer::device->CreateRasterizerState(&rasterizerDesc, &rasterShadow);
	}

	//初始化rasterCullBack
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.DepthClipEnable = TRUE;

		Renderer::device->CreateRasterizerState(&rasterizerDesc, &rasterCullBack);
	}

	//初始化rasterCullFront
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		rasterizerDesc.DepthClipEnable = TRUE;

		Renderer::device->CreateRasterizerState(&rasterizerDesc, &rasterCullFront);
	}

	//初始化rasterCullNone
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthClipEnable = TRUE;

		Renderer::device->CreateRasterizerState(&rasterizerDesc, &rasterCullNone);
	}

	//初始化rasterConserve
	{
		D3D11_RASTERIZER_DESC2 rasterizerDesc = {};
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.ForcedSampleCount = 8;
		rasterizerDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_ON;

		Renderer::device->CreateRasterizerState2(&rasterizerDesc, &rasterConserve);
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

		Renderer::device->CreateDepthStencilState(&dsDesc, &defDepthStencilState);
	}

	//初始化depthStencilDisable
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = false;
		dsDesc.StencilEnable = false;

		Renderer::device->CreateDepthStencilState(&dsDesc, &depthStencilDisable);
	}
}

States::~States()
{
	anisotropicClampSampler->Release();
	anisotropicWrapSampler->Release();
	linearClampSampler->Release();
	linearWrapSampler->Release();
	pointClampSampler->Release();
	pointWrapSampler->Release();
	shadowSampler->Release();
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
