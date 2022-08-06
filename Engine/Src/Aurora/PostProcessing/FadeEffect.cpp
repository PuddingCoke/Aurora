#include<Aurora/PostProcessing/FadeEffect.h>

FadeEffect::FadeEffect(RenderTexture* defRenderTexture):
	EffectBase(defRenderTexture)
{
	compileShaders();

	D3D11_BLEND_DESC blendStateDesc = {};

	blendStateDesc.IndependentBlendEnable = false;

	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_REV_SUBTRACT;

	Graphics::device->CreateBlendState(&blendStateDesc, fadeBlendState.ReleaseAndGetAddressOf());

}

void FadeEffect::process() const
{
	Graphics::setBlendState(fadeBlendState.Get());

	defRenderTexture->setMSAARTV();

	Shader::displayVShader->use();
	fadePShader->use();

	Graphics::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::context->Draw(3, 0);
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

float4 main() : SV_TARGET
{
    return float4(0.0, 0.0, 0.0, 5.0*deltaTime);
}
			)";
		fadePShader = Shader::fromStr(source, ShaderType::Pixel);
	}
}
