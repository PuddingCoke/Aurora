#include<Aurora/Effect/FadeEffect.h>

FadeEffect::FadeEffect(GraphicsContext* const ctx, const unsigned int& width, const unsigned int& height) :
	EffectBase(ctx, width, height, FMT::RGBA16F), fadeParam{ 3.f,0.f,0.f,0.f },
	fadeBuffer(new ConstantBuffer(sizeof(FadeParam), D3D11_USAGE_DYNAMIC))
{
	compileShaders();

	setFadeSpeed(3.f);
}

ShaderResourceView* FadeEffect::process(ShaderResourceView* const texture2D) const
{
	ctx->OMSetBlendState(nullptr);

	ctx->ClearRTV(outputRTV->getMip(0), DirectX::Colors::Black);
	ctx->OMSetRTV({ outputRTV->getMip(0) }, nullptr);

	ctx->PSSetConstantBuffer({ fadeBuffer }, 1);
	ctx->PSSetSampler({ States::linearClampSampler }, 0);
	ctx->PSSetSRV({ texture2D }, 0);

	ctx->BindShader(Shader::fullScreenVS);
	ctx->BindShader(fadePShader);

	ctx->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->DrawQuad();

	return outputRTV;
}

const float& FadeEffect::getFadeSpeed() const
{
	return fadeParam.fadeSpeed;
}

void FadeEffect::setFadeSpeed(const float& speed)
{
	fadeParam.fadeSpeed = speed;

	BufferUpdate::pushBufferUpdateParam(fadeBuffer, &fadeParam, sizeof(FadeParam));
}

FadeEffect::~FadeEffect()
{
	delete fadePShader;
	delete fadeBuffer;
}

void FadeEffect::compileShaders()
{
	fadePShader = new Shader(g_FadePSBytes, sizeof(g_FadePSBytes), ShaderType::Pixel);
}
