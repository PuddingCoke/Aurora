#include<Aurora/PostProcessing/FadeEffect.h>

FadeEffect::FadeEffect(const unsigned int& width, const unsigned int& height) :
	EffectBase(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT), fadeFactor(3.f)
{
	compileShaders();

	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = 16u;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Renderer::device->CreateBuffer(&bd, nullptr, fadeBuffer.GetAddressOf());

	setFadeFactor(fadeFactor);
}

ShaderResourceView* FadeEffect::process(ShaderResourceView* const texture2D) const
{
	Renderer::setBlendState(nullptr);

	outputRTV->clearRTV(DirectX::Colors::Black);
	ResManager::get()->OMSetRTV({ outputRTV }, nullptr);

	Renderer::context->PSSetConstantBuffers(3, 1, fadeBuffer.GetAddressOf());

	Renderer::context->PSSetSamplers(0, 1, States::get()->linearClampSampler.GetAddressOf());
	ResManager::get()->PSSetSRV({ texture2D }, 0);

	Shader::displayVShader->use();
	fadePShader->use();

	Renderer::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Renderer::context->Draw(3, 0);

	return outputRTV;
}

const float& FadeEffect::getFadeFactor() const
{
	return fadeFactor;
}

void FadeEffect::setFadeFactor(const float& factor)
{
	fadeFactor = factor;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(fadeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &fadeFactor, sizeof(float));
	Renderer::context->Unmap(fadeBuffer.Get(), 0);
}

FadeEffect::~FadeEffect()
{
	delete fadePShader;
}

void FadeEffect::compileShaders()
{
	{
		const std::string source = R"(
cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float v1;
    float v2;
    float v3;
};

cbuffer FadeFactor : register(b3)
{
	float factor;
}

SamplerState linearSampler : register(s0);

Texture2D tTexture : register(t0);

float4 main(float2 texCoord:TEXCOORD) : SV_TARGET
{
    float3 color = tTexture.Sample(linearSampler, texCoord).rgb;
    float3 normColor = normalize(color);
	float3 fadeColor = saturate(color - factor * deltaTime);
    return float4(fadeColor, 1.0f);
}
			)";
		fadePShader = Shader::fromStr(source, ShaderType::Pixel);
	}
}
