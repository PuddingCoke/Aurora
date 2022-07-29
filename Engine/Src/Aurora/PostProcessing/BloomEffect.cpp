#include<Aurora/PostProcessing/BloomEffect.h>

BloomEffect::BloomEffect(const unsigned int& width, const unsigned int& height) :
	EffectBase(width, height), bloomWidth(width), bloomHeight(height)
{
	compileShaders();

	originTexture = RenderTexture::create(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Transparent, false);

	bloomTexture = RenderTexture::create(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Transparent, false);

	{
		unsigned int currentWidth = width;
		unsigned int currentHeight = height;
		for (unsigned int i = 0; i < blurSteps; i++)
		{
			currentWidth /= 2;
			currentHeight /= 2;

			resolutionX[i] = currentWidth;
			resolutionY[i] = currentHeight;

			renderTextures[i * 2u] = RenderTexture::create(currentWidth, currentHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Transparent, false);
			renderTextures[i * 2u + 1u] = RenderTexture::create(currentWidth, currentHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Transparent, false);

		}
	}

}

BloomEffect::~BloomEffect()
{
	for (unsigned int i = 0; i < blurSteps; i++)
	{
		delete renderTextures[i * 2u];
		delete renderTextures[i * 2u + 1u];
	}

	delete originTexture;
	delete bloomTexture;

	delete bloomExtract;
	delete bloomFinal;

	for (unsigned int i = 0; i < blurSteps; i++)
	{
		delete blurShaders[i * 2u];
		delete blurShaders[i * 2u + 1u];
	}
}

Texture2D* BloomEffect::process(Texture2D* const texture2D) const
{
	Graphics::setBlendState(StateCommon::addtiveBlend.Get());
	Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());
	Shader::displayVShader->use();

	ID3D11ShaderResourceView* const nullSRV = nullptr;

	Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

	bloomExtract->use();
	Graphics::setViewport((float)bloomWidth, (float)bloomHeight);
	RenderTexture::setRTVs({ originTexture,bloomTexture });
	originTexture->clearRTV(DirectX::Colors::Transparent);
	bloomTexture->clearRTV(DirectX::Colors::Transparent);
	texture2D->setSRV(0);
	Graphics::context->Draw(3, 0);

	Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

	Shader::displayPShader->use();
	Graphics::setViewport((float)resolutionX[0], (float)resolutionY[0]);
	renderTextures[0]->clearRTV(DirectX::Colors::Transparent);
	renderTextures[0]->setRTV();
	bloomTexture->getTexture()->setSRV(0);
	Graphics::context->Draw(3, 0);

	Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		blurShaders[i * 2]->use();
		renderTextures[i * 2 + 1]->clearRTV(DirectX::Colors::Transparent);
		renderTextures[i * 2 + 1]->setRTV();
		renderTextures[i * 2]->getTexture()->setSRV(0);
		Graphics::context->Draw(3, 0);

		Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

		blurShaders[i * 2 + 1]->use();
		renderTextures[i * 2]->clearRTV(DirectX::Colors::Transparent);
		renderTextures[i * 2]->setRTV();
		renderTextures[i * 2 + 1]->getTexture()->setSRV(0);
		Graphics::context->Draw(3, 0);

		Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

		Graphics::setViewport((float)resolutionX[i + 1], (float)resolutionY[i + 1]);

		Shader::displayPShader->use();
		renderTextures[i * 2 + 2]->clearRTV(DirectX::Colors::Transparent);
		renderTextures[i * 2 + 2]->setRTV();
		renderTextures[i * 2]->getTexture()->setSRV(0);
		Graphics::context->Draw(3, 0);

		Graphics::context->PSSetShaderResources(0, 1, &nullSRV);
	}

	blurShaders[(blurSteps - 1) * 2]->use();
	renderTextures[(blurSteps - 1) * 2 + 1]->clearRTV(DirectX::Colors::Transparent);
	renderTextures[(blurSteps - 1) * 2 + 1]->setRTV();
	renderTextures[(blurSteps - 1) * 2]->getTexture()->setSRV(0);
	Graphics::context->Draw(3, 0);

	Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

	blurShaders[(blurSteps - 1) * 2 + 1]->use();
	renderTextures[(blurSteps - 1) * 2]->clearRTV(DirectX::Colors::Transparent);
	renderTextures[(blurSteps - 1) * 2]->setRTV();
	renderTextures[(blurSteps - 1) * 2 + 1]->getTexture()->setSRV(0);
	Graphics::context->Draw(3, 0);

	Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

	blurShaders[(blurSteps - 1) * 2]->use();
	renderTextures[(blurSteps - 1) * 2 + 1]->clearRTV(DirectX::Colors::Transparent);
	renderTextures[(blurSteps - 1) * 2 + 1]->setRTV();
	renderTextures[(blurSteps - 1) * 2]->getTexture()->setSRV(0);
	Graphics::context->Draw(3, 0);

	Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

	blurShaders[(blurSteps - 1) * 2 + 1]->use();
	renderTextures[(blurSteps - 1) * 2]->clearRTV(DirectX::Colors::Transparent);
	renderTextures[(blurSteps - 1) * 2]->setRTV();
	renderTextures[(blurSteps - 1) * 2 + 1]->getTexture()->setSRV(0);
	Graphics::context->Draw(3, 0);

	Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		Graphics::setViewport((float)resolutionX[blurSteps - 2 - i], (float)resolutionY[blurSteps - 2 - i]);

		blurShaders[(blurSteps - 2 - i) * 2]->use();
		renderTextures[(blurSteps - 2 - i) * 2 + 1]->clearRTV(DirectX::Colors::Transparent);
		renderTextures[(blurSteps - 2 - i) * 2 + 1]->setRTV();
		renderTextures[(blurSteps - 2 - i) * 2]->getTexture()->setSRV(0);
		Graphics::context->Draw(3, 0);

		Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

		blurShaders[(blurSteps - 2 - i) * 2 + 1]->use();
		renderTextures[(blurSteps - 2 - i) * 2]->clearRTV(DirectX::Colors::Transparent);
		renderTextures[(blurSteps - 2 - i) * 2]->setRTV();
		renderTextures[(blurSteps - 2 - i) * 2 + 1]->getTexture()->setSRV(0);
		Graphics::context->Draw(3, 0);

		Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

		Shader::displayPShader->use();
		renderTextures[(blurSteps - 2 - i) * 2]->setRTV();
		renderTextures[(blurSteps - 1 - i) * 2]->getTexture()->setSRV();
		Graphics::context->Draw(3, 0);

		Graphics::context->PSSetShaderResources(0, 1, &nullSRV);
	}

	Graphics::setViewport((float)bloomWidth, (float)bloomHeight);
	originTexture->setRTV();
	renderTextures[0]->getTexture()->setSRV();
	Graphics::context->Draw(3, 0);

	Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

	bloomFinal->use();
	defRenderTexture->clearRTV(DirectX::Colors::Transparent);
	defRenderTexture->setRTV();
	originTexture->getTexture()->setSRV(0);
	Graphics::context->Draw(3, 0);

	RenderTexture::unbindAll();
	Graphics::context->PSSetShaderResources(0, 1, &nullSRV);

	return outputTexture;
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

PixelOutput main(float2 texCoord : TEXCOORD)
{
    PixelOutput output;
    const float4 color = tTexture.Sample(samplerState, texCoord);
    output.color = color;
    if (dot(color.rgb, float3(0.2126, 0.7152, 0.0722))>1.0)
    {
        output.brightColor = color;
    }
    else
    {
        output.brightColor = float4(0.0, 0.0, 0.0, 1.0);
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

static const float exposure = 0.36;
static const float gamma = 1.0f;

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
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

static const float weight[2] = { 0.72534960073, 0.138900963246 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 2; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(tex_offset.x * i, 0.0)).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[0] = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

static const float weight[2] = { 0.72534960073, 0.138900963246 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 2; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(0.0, tex_offset.y * i)).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[1] = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[3] = { 0.0, 1.0, 2.0 };

//static const float weight[3] = { 0.474931286192, 0.233820412529, 0.0279021295185 };

static const float offsets[2] = { 0.0, 1.10660957707432 };

static const float weight[2] = { 0.474931286192, 0.2617225420475 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 2; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[2] = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[3] = { 0.0, 1.0, 2.0 };

//static const float weight[3] = { 0.474931286192, 0.233820412529, 0.0279021295185 };

static const float offsets[2] = { 0.0, 1.10660957707432 };

static const float weight[2] = { 0.474931286192, 0.2617225420475 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 2; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[3] = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[4] = { 0.0, 1.0, 2.0, 3.0 };

//static const float weight[4] = { 0.346906330784, 0.237693723707, 0.0764599801617, 0.0115467995181 };

static const float offsets[3] = { 0.0, 1.243383984400, 3.0 };

static const float weight[3] = { 0.346906330784, 0.3141537038687, 0.0115467995181 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 3; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[4] = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[4] = { 0.0, 1.0, 2.0, 3.0 };

//static const float weight[4] = { 0.346906330784, 0.237693723707, 0.0764599801617, 0.0115467995181 };

static const float offsets[3] = { 0.0, 1.243383984400, 3.0 };

static const float weight[3] = { 0.346906330784, 0.3141537038687, 0.0115467995181 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 3; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[5] = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[5] = { 0.0, 1.0, 2.0, 3.0, 4.0 };

//static const float weight[5] = { 0.209969621264, 0.182812073509, 0.120656327901, 0.0603659656484, 0.0228944924579 };

static const float offsets[3] = { 0.0, 1.397591074854, 3.274974375335 };

static const float weight[3] = { 0.209969621264, 0.30346840141, 0.0832604581063 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 3; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[6] = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[5] = { 0.0, 1.0, 2.0, 3.0, 4.0 };

//static const float weight[5] = { 0.209969621264, 0.182812073509, 0.120656327901, 0.0603659656484, 0.0228944924579 };

static const float offsets[3] = { 0.0, 1.397591074854, 3.274974375335 };

static const float weight[3] = { 0.209969621264, 0.30346840141, 0.0832604581063 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 3; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[7] = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[6] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 };

//static const float weight[6] = { 0.120891600122, 0.115466517369, 0.100608781811, 0.0799715652254, 0.0579902771337, 0.0383614182091 };

static const float offsets[4] = { 0.0, 1.465619079056, 3.420335624271, 5.0 };

static const float weight[4] = { 0.120891600122, 0.21607529918, 0.1379618423591, 0.0383614182091 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 4; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[8] = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[6] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 };

//static const float weight[6] = { 0.120891600122, 0.115466517369, 0.100608781811, 0.0799715652254, 0.0579902771337, 0.0383614182091 };

static const float offsets[4] = { 0.0, 1.465619079056, 3.420335624271, 5.0 };

static const float weight[4] = { 0.120891600122, 0.21607529918, 0.1379618423591, 0.0383614182091 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 4; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}
)";

		blurShaders[9] = Shader::fromStr(source, ShaderType::Pixel);
	}
}
