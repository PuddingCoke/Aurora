#include<Aurora/PostProcessing/BloomEffect.h>

BloomEffect::BloomEffect(const unsigned int& width, const unsigned int& height) :
	EffectBase(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT), bloomWidth(width), bloomHeight(height), bloomParam{},
	originTexture(new RenderTexture(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
	bloomTexture(new RenderTexture(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black))
{
	compileShaders();

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

			rcTextures[i * 2u] = new RCTexture(resolutions[i].x, resolutions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black);
			rcTextures[i * 2u + 1u] = new RCTexture(resolutions[i].x, resolutions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black);

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

			blurParamBuffer[i] = new Buffer(sizeof(BlurParam), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE, &blurParam, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(BlurParam));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = 1;

			blurParamSRV[i] = new ShaderResourceView(blurParamBuffer[i]->getBuffer(), srvDesc);
		}
	}

	bloomParamBuffer = new Buffer(sizeof(BloomParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	bloomParam.exposure = 0.36f;
	bloomParam.gamma = 1.0f;
	bloomParam.threshold = 1.0f;
	bloomParam.intensity = 1.0f;

	applyChange();
}

BloomEffect::~BloomEffect()
{
	for (unsigned int i = 0; i < blurSteps; i++)
	{
		delete rcTextures[i * 2u];
		delete rcTextures[i * 2u + 1u];
		delete blurParamBuffer[i];
		delete blurParamSRV[i];
	}

	delete bloomParamBuffer;

	delete originTexture;
	delete bloomTexture;

	delete bloomExtract;
	delete bloomFinal;

	delete bloomHBlurShader;
	delete bloomVBlurShader;
}

ShaderResourceView* BloomEffect::process(ShaderResourceView* const texture2D) const
{
	RenderAPI::get()->OMSetBlendState(nullptr);
	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderAPI::get()->PSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);
	RenderAPI::get()->CSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);
	RenderAPI::get()->PSSetBuffer({ bloomParamBuffer }, 1);
	RenderAPI::get()->CSSetBuffer({ bloomParamBuffer }, 1);
	Shader::displayVShader->use();

	bloomExtract->use();
	RenderAPI::get()->RSSetViewport(bloomWidth, bloomHeight);
	RenderAPI::get()->OMSetRTV({ originTexture,bloomTexture }, nullptr);
	RenderAPI::get()->PSSetSRV({ texture2D }, 0);
	RenderAPI::get()->DrawQuad();

	Shader::displayPShader->use();
	RenderAPI::get()->RSSetViewport(resolutions[0].x, resolutions[0].y);
	RenderAPI::get()->OMSetRTV({ rcTextures[0] }, nullptr);
	RenderAPI::get()->PSSetSRV({ bloomTexture }, 0);
	RenderAPI::get()->DrawQuad();

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		RenderAPI::get()->CSSetSRV({ blurParamSRV[i] }, 1);

		bloomHBlurShader->use();
		RenderAPI::get()->CSSetUAV({ rcTextures[i * 2 + 1] }, 0);
		RenderAPI::get()->CSSetSRV({ rcTextures[i * 2] }, 0);
		RenderAPI::get()->Dispatch(rcTextures[i * 2]->getWidth() / workGroupSize.x, rcTextures[i * 2]->getHeight() / workGroupSize.y + 1, 1);

		bloomVBlurShader->use();
		RenderAPI::get()->CSSetUAV({ rcTextures[i * 2] }, 0);
		RenderAPI::get()->CSSetSRV({ rcTextures[i * 2 + 1] }, 0);
		RenderAPI::get()->Dispatch(rcTextures[i * 2]->getWidth() / workGroupSize.x, rcTextures[i * 2]->getHeight() / workGroupSize.y + 1, 1);

		RenderAPI::get()->RSSetViewport(resolutions[i + 1].x, resolutions[i + 1].y);

		Shader::displayPShader->use();
		RenderAPI::get()->OMSetRTV({ rcTextures[i * 2 + 2] }, nullptr);
		RenderAPI::get()->PSSetSRV({ rcTextures[i * 2] }, 0);
		RenderAPI::get()->DrawQuad();
	}

	RenderAPI::get()->CSSetSRV({ blurParamSRV[blurSteps - 1] }, 1);

	for (int i = 0; i < 2; i++)
	{
		bloomHBlurShader->use();
		RenderAPI::get()->CSSetUAV({ rcTextures[(blurSteps - 1) * 2 + 1] }, 0);
		RenderAPI::get()->CSSetSRV({ rcTextures[(blurSteps - 1) * 2] }, 0);
		RenderAPI::get()->Dispatch(rcTextures[(blurSteps - 1) * 2]->getWidth() / workGroupSize.x, rcTextures[(blurSteps - 1) * 2]->getHeight() / workGroupSize.y + 1, 1);

		bloomVBlurShader->use();
		RenderAPI::get()->CSSetUAV({ rcTextures[(blurSteps - 1) * 2] }, 0);
		RenderAPI::get()->CSSetSRV({ rcTextures[(blurSteps - 1) * 2 + 1] }, 0);
		RenderAPI::get()->Dispatch(rcTextures[(blurSteps - 1) * 2]->getWidth() / workGroupSize.x, rcTextures[(blurSteps - 1) * 2]->getHeight() / workGroupSize.y + 1, 1);
	}

	RenderAPI::get()->OMSetBlendState(States::get()->addtiveBlend.Get());

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		RenderAPI::get()->RSSetViewport(resolutions[blurSteps - 2 - i].x, resolutions[blurSteps - 2 - i].y);
		RenderAPI::get()->CSSetSRV({ blurParamSRV[blurSteps - 2 - i] }, 1);

		bloomHBlurShader->use();
		RenderAPI::get()->CSSetUAV({ rcTextures[(blurSteps - 2 - i) * 2 + 1] }, 0);
		RenderAPI::get()->CSSetSRV({ rcTextures[(blurSteps - 2 - i) * 2] }, 0);
		RenderAPI::get()->Dispatch(rcTextures[(blurSteps - 2 - i) * 2]->getWidth() / workGroupSize.x, rcTextures[(blurSteps - 2 - i) * 2]->getHeight() / workGroupSize.y + 1, 1);

		bloomVBlurShader->use();
		RenderAPI::get()->CSSetUAV({ rcTextures[(blurSteps - 2 - i) * 2] }, 0);
		RenderAPI::get()->CSSetSRV({ rcTextures[(blurSteps - 2 - i) * 2 + 1] }, 0);
		RenderAPI::get()->Dispatch(rcTextures[(blurSteps - 2 - i) * 2]->getWidth() / workGroupSize.x, rcTextures[(blurSteps - 2 - i) * 2]->getHeight() / workGroupSize.y + 1, 1);

		Shader::displayPShader->use();
		RenderAPI::get()->OMSetRTV({ rcTextures[(blurSteps - 2 - i) * 2] }, nullptr);
		RenderAPI::get()->PSSetSRV({ rcTextures[(blurSteps - 1 - i) * 2] }, 0);
		RenderAPI::get()->DrawQuad();
	}

	RenderAPI::get()->RSSetViewport(bloomWidth, bloomHeight);
	RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);
	RenderAPI::get()->PSSetSRV({ rcTextures[0] }, 0);
	RenderAPI::get()->DrawQuad();

	bloomFinal->use();
	outputRTV->clearRTV(DirectX::Colors::Black);
	RenderAPI::get()->OMSetRTV({ outputRTV }, nullptr);
	RenderAPI::get()->PSSetSRV({ originTexture }, 0);
	RenderAPI::get()->DrawQuad();

	return outputRTV;
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
	bloomParamBuffer->updateSubresource(STRUCTDATA(bloomParam), 0);
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
