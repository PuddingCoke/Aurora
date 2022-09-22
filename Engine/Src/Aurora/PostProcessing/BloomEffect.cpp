#include<Aurora/PostProcessing/BloomEffect.h>

BloomEffect::BloomEffect(const unsigned int& width, const unsigned int& height) :
	EffectBase(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT), bloomWidth(width), bloomHeight(height), bloomParam{},
	originTexture(RenderTexture::create(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
	bloomTexture(RenderTexture::create(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black))
{
	compileShaders();

	bloomParam.exposure = 0.36f;
	bloomParam.gamma = 1.0f;
	bloomParam.threshold = 1.0f;
	bloomParam.intensity = 1.0f;

	{

		const float weights[blurSteps][4] =
		{
			{0.72534960073f, 0.138900963246f,0.f,0.f},
			{0.474931286192f, 0.2617225420475f,0.f,0.f},
			{0.346906330784f, 0.3141537038687f, 0.0115467995181f,0.f},
			{0.209969621264f, 0.30346840141f, 0.0832604581063f,0.f},
			{0.120891600122f, 0.21607529918f, 0.1379618423591f, 0.0383614182091f}
		};

		const float offsets[blurSteps][4] =
		{
			{0.f,1.f,0.f,0.f},
			{0.f,1.10660957707432f,0.f,0.f},
			{0.f,1.243383984400f, 3.f,0.f},
			{0.f,1.397591074854f, 3.274974375335f,0.f},
			{0.f,1.465619079056f, 3.420335624271f, 5.f}
		};

		const int iterations[blurSteps] = { 2,2,3,3,4 };

		for (unsigned int i = 0; i < blurSteps; i++)
		{
			resolutions[i] = DirectX::XMUINT2(width >> (i + 1), height >> (i + 1));

			rwTextures[i * 2u] = RWTexture::create(resolutions[i].x, resolutions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black);
			rwTextures[i * 2u + 1u] = RWTexture::create(resolutions[i].x, resolutions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black);

			struct BlurParam
			{
				float weight[4];
				float offset[4];
				DirectX::XMFLOAT2 texelSize;
				int iteration;
				float v0;
			}blurParam{};

			for (int j = 0; j < 4; j++)
			{
				blurParam.weight[j] = weights[i][j];
				blurParam.offset[j] = offsets[i][j];
			}

			blurParam.texelSize = DirectX::XMFLOAT2(1.f / resolutions[i].x, 1.f / resolutions[i].y);
			blurParam.iteration = iterations[i];

			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(BlurParam);
			//也许可以在程序运行中动态调整weight和offset
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bd.StructureByteStride = sizeof(BlurParam);

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = &blurParam;

			Renderer::device->CreateBuffer(&bd, &data, blurParamBuffer[i].ReleaseAndGetAddressOf());

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = 1;

			Renderer::device->CreateShaderResourceView(blurParamBuffer[i].Get(), &srvDesc, blurParamSRV[i].ReleaseAndGetAddressOf());
		}
	}

	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = sizeof(BloomParam);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Renderer::device->CreateBuffer(&cbd, nullptr, bloomParamBuffer.ReleaseAndGetAddressOf());

		applyChange();
	}
}

BloomEffect::~BloomEffect()
{
	for (unsigned int i = 0; i < blurSteps; i++)
	{
		delete rwTextures[i * 2u];
		delete rwTextures[i * 2u + 1u];
	}

	delete originTexture;
	delete bloomTexture;

	delete bloomExtract;
	delete bloomFinal;

	delete bloomHBlurShader;
	delete bloomVBlurShader;
}

Texture2D* BloomEffect::process(Texture2D* const texture2D) const
{
	Renderer::setBlendState(StateCommon::addtiveBlend.Get());
	Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Renderer::context->PSSetSamplers(0, 1, StateCommon::linearClampSampler.GetAddressOf());
	Renderer::context->CSSetSamplers(0, 1, StateCommon::linearClampSampler.GetAddressOf());
	Renderer::context->PSSetConstantBuffers(1, 1, bloomParamBuffer.GetAddressOf());
	Renderer::context->CSSetConstantBuffers(1, 1, bloomParamBuffer.GetAddressOf());
	Shader::displayVShader->use();

	ID3D11ShaderResourceView* const nullSRV[1] = { nullptr };
	ID3D11RenderTargetView* const nullRTV[1] = { nullptr };
	ID3D11UnorderedAccessView* const nullUAV[1] = { nullptr };

	bloomExtract->use();
	Renderer::setViewport(bloomWidth, bloomHeight);
	RenderTexture::setRTVs({ originTexture,bloomTexture });
	originTexture->clearRTV(DirectX::Colors::Black);
	bloomTexture->clearRTV(DirectX::Colors::Black);
	texture2D->setSRV();
	Renderer::drawQuad();

	Shader::displayPShader->use();
	Renderer::setViewport(resolutions[0].x, resolutions[0].y);
	rwTextures[0]->clearRTV(DirectX::Colors::Black);
	rwTextures[0]->setRTV();
	bloomTexture->getTexture()->setSRV();
	Renderer::drawQuad();

	Renderer::context->OMSetRenderTargets(1, nullRTV, nullptr);

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		Renderer::context->CSSetShaderResources(1, 1, blurParamSRV[i].GetAddressOf());

		bloomHBlurShader->use();
		rwTextures[i * 2 + 1]->setUAV();
		rwTextures[i * 2]->CSSetSRV();
		Renderer::context->Dispatch(rwTextures[i * 2]->width / workGroupSize.x, rwTextures[i * 2]->height / workGroupSize.y + 1, 1);

		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		bloomVBlurShader->use();
		rwTextures[i * 2]->setUAV();
		rwTextures[i * 2 + 1]->CSSetSRV();
		Renderer::context->Dispatch(rwTextures[i * 2]->width / workGroupSize.x, rwTextures[i * 2]->height / workGroupSize.y + 1, 1);

		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

		Renderer::setViewport(resolutions[i + 1].x, resolutions[i + 1].y);

		Shader::displayPShader->use();
		rwTextures[i * 2 + 2]->clearRTV(DirectX::Colors::Black);
		rwTextures[i * 2 + 2]->setRTV();
		rwTextures[i * 2]->PSSetSRV();
		Renderer::drawQuad();

		Renderer::context->OMSetRenderTargets(1, nullRTV, nullptr);
	}

	Renderer::context->PSSetShaderResources(0, 1, nullSRV);

	Renderer::context->CSSetShaderResources(1, 1, blurParamSRV[blurSteps - 1].GetAddressOf());

	for (int i = 0; i < 2; i++)
	{
		bloomHBlurShader->use();
		rwTextures[(blurSteps - 1) * 2 + 1]->setUAV();
		rwTextures[(blurSteps - 1) * 2]->CSSetSRV();
		Renderer::context->Dispatch(rwTextures[(blurSteps - 1) * 2]->width / workGroupSize.x, rwTextures[(blurSteps - 1) * 2]->height / workGroupSize.y + 1, 1);

		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		bloomVBlurShader->use();
		rwTextures[(blurSteps - 1) * 2]->setUAV();
		rwTextures[(blurSteps - 1) * 2 + 1]->CSSetSRV();
		Renderer::context->Dispatch(rwTextures[(blurSteps - 1) * 2]->width / workGroupSize.x, rwTextures[(blurSteps - 1) * 2]->height / workGroupSize.y + 1, 1);

		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		Renderer::setViewport(resolutions[blurSteps - 2 - i].x, resolutions[blurSteps - 2 - i].y);
		Renderer::context->CSSetShaderResources(1, 1, blurParamSRV[blurSteps - 2 - i].GetAddressOf());

		bloomHBlurShader->use();
		rwTextures[(blurSteps - 2 - i) * 2 + 1]->setUAV();
		rwTextures[(blurSteps - 2 - i) * 2]->CSSetSRV();
		Renderer::context->Dispatch(rwTextures[(blurSteps - 2 - i) * 2]->width / workGroupSize.x, rwTextures[(blurSteps - 2 - i) * 2]->height / workGroupSize.y + 1, 1);

		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		bloomVBlurShader->use();
		rwTextures[(blurSteps - 2 - i) * 2]->setUAV();
		rwTextures[(blurSteps - 2 - i) * 2 + 1]->CSSetSRV();
		Renderer::context->Dispatch(rwTextures[(blurSteps - 2 - i) * 2]->width / workGroupSize.x, rwTextures[(blurSteps - 2 - i) * 2]->height / workGroupSize.y + 1, 1);

		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

		Shader::displayPShader->use();
		rwTextures[(blurSteps - 2 - i) * 2]->setRTV();
		rwTextures[(blurSteps - 1 - i) * 2]->PSSetSRV();
		Renderer::drawQuad();

		Renderer::context->PSSetShaderResources(0, 1, nullSRV);
	}

	Renderer::setViewport(bloomWidth, bloomHeight);
	originTexture->setRTV();
	rwTextures[0]->PSSetSRV();
	Renderer::drawQuad();

	bloomFinal->use();
	outputRTV->clearRTV(DirectX::Colors::Black);
	outputRTV->setRTV();
	originTexture->getTexture()->setSRV(0);
	Renderer::drawQuad();

	Renderer::context->CSSetShader(nullptr, nullptr, 0);
	Renderer::context->OMSetRenderTargets(1, nullRTV, nullptr);
	Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	Renderer::context->PSSetShaderResources(0, 1, nullSRV);
	Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	return outputRTV->getTexture();
}

void BloomEffect::setExposure(const float& exposure)
{
	bloomParam.exposure = exposure;
}

void BloomEffect::setGamma(const float& gamma)
{
	bloomParam.gamma = gamma;
}

void BloomEffect::setThreshold(const float& threshold)
{
	bloomParam.threshold = threshold;
}

void BloomEffect::setIntensity(const float& intensity)
{
	bloomParam.intensity = intensity;
}

const float& BloomEffect::getExposure() const
{
	return bloomParam.exposure;
}

const float& BloomEffect::getGamma() const
{
	return bloomParam.gamma;
}

const float& BloomEffect::getThreshold() const
{
	return bloomParam.threshold;
}

const float& BloomEffect::getIntensity() const
{
	return bloomParam.intensity;
}

void BloomEffect::applyChange() const
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(bloomParamBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &bloomParam, sizeof(BloomParam));
	Renderer::context->Unmap(bloomParamBuffer.Get(), 0);
}

void BloomEffect::compileShaders()
{
	{
		const std::string source = R"(
struct PixelOutput
{
    float4 color : SV_TARGET0;
    float4 brightColor : SV_TARGET1;
};

Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
};

PixelOutput main(float2 texCoord : TEXCOORD)
{
    PixelOutput output;
    const float4 color = tTexture.Sample(samplerState, texCoord);
    output.color = color;
	output.brightColor = float4(0.0, 0.0, 0.0, 1.0);
    if (dot(color.rgb, float3(0.2126, 0.7152, 0.0722))>threshold)
    {
        output.brightColor = color;
    }
    return output;
}
)";

		bloomExtract = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
};

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 hdrColor = tTexture.Sample(samplerState, texCoord).rgb;
    float3 result = float3(1.0, 1.0, 1.0) - exp(-hdrColor * exposure);
    result = pow(result, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
    return float4(result, 1.0);
}
)";

		bloomFinal = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
};

struct BlurParams
{
    float weight[4];
    float offset[4];
    float2 texelSize;
    int iteration;
    float v0;
};

Texture2D sourceTexture : register(t0);
StructuredBuffer<BlurParams> blurParams : register(t1);

RWTexture2D<float4> destTexture : register(u0);

SamplerState linearSampler : register(s0);

[numthreads(60, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    const BlurParams param = blurParams.Load(0);
    
    const float2 texCoord = (float2(DTid.xy) + 0.5) * param.texelSize;
    
    float3 result = sourceTexture.SampleLevel(linearSampler, texCoord, 0.0).rgb * param.weight[0];
    
    [unroll]
    for (int i = 1; i < param.iteration; ++i)
    {
        result += sourceTexture.SampleLevel(linearSampler, texCoord + float2(param.texelSize.x * param.offset[i], 0.0), 0.0).rgb * param.weight[i];
        result += sourceTexture.SampleLevel(linearSampler, texCoord - float2(param.texelSize.x * param.offset[i], 0.0), 0.0).rgb * param.weight[i];
    }
    
    destTexture[DTid.xy] = float4(intensity * result, 1.0);
}
)";

		bloomHBlurShader = Shader::fromStr(source, ShaderType::Compute);
	}

	{
		const std::string source = R"(
cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
};

struct BlurParams
{
    float weight[4];
    float offset[4];
    float2 texelSize;
    int iteration;
    float v0;
};

Texture2D sourceTexture : register(t0);
StructuredBuffer<BlurParams> blurParams : register(t1);

RWTexture2D<float4> destTexture : register(u0);

SamplerState linearSampler : register(s0);

[numthreads(60, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    const BlurParams param = blurParams.Load(0);
    
    const float2 texCoord = (float2(DTid.xy) + 0.5) * param.texelSize;
    
    float3 result = sourceTexture.SampleLevel(linearSampler, texCoord, 0.0).rgb * param.weight[0];
    
    [unroll]
    for (int i = 1; i < param.iteration; ++i)
    {
        result += sourceTexture.SampleLevel(linearSampler, texCoord + float2(0.0, param.texelSize.y * param.offset[i]), 0.0).rgb * param.weight[i];
        result += sourceTexture.SampleLevel(linearSampler, texCoord - float2(0.0, param.texelSize.y * param.offset[i]), 0.0).rgb * param.weight[i];
    }
    
    destTexture[DTid.xy] = float4(intensity * result, 1.0);
}
)";

		bloomVBlurShader = Shader::fromStr(source, ShaderType::Compute);
	}
}
