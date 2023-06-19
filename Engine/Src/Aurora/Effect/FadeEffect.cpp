#include<Aurora/Effect/FadeEffect.h>

FadeEffect::FadeEffect(const unsigned int& width, const unsigned int& height) :
	EffectBase(width, height, FMT::RGBA16F), fadeParam{ 3.f,0.f,0.f,0.f },
	fadeBuffer(new ConstantBuffer(sizeof(FadeParam), D3D11_USAGE_DYNAMIC))
{
	compileShaders();

	setFadeSpeed(3.f);
}

ShaderResourceView* FadeEffect::process(ShaderResourceView* const texture2D) const
{
	ImCtx::get()->OMSetBlendState(nullptr);

	ImCtx::get()->ClearRTV(outputRTV->getMip(0), DirectX::Colors::Black);
	ImCtx::get()->OMSetRTV({ outputRTV->getMip(0) }, nullptr);

	ImCtx::get()->PSSetConstantBuffer({ fadeBuffer }, 1);
	ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);
	ImCtx::get()->PSSetSRV({ texture2D }, 0);

	ImCtx::get()->BindShader(ImCtx::fullScreenVS);
	ImCtx::get()->BindShader(fadePShader);

	ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ImCtx::get()->DrawQuad();

	return outputRTV;
}

const float& FadeEffect::getFadeSpeed() const
{
	return fadeParam.fadeSpeed;
}

void FadeEffect::setFadeSpeed(const float& speed)
{
	fadeParam.fadeSpeed = speed;

	memcpy(ImCtx::get()->Map(fadeBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &fadeParam, sizeof(FadeParam));
	ImCtx::get()->Unmap(fadeBuffer, 0);
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
