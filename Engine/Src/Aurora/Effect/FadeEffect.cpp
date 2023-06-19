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
	RenderAPI::get()->OMSetBlendState(nullptr);

	outputRTV->clearRTV(DirectX::Colors::Black, 0);
	RenderAPI::get()->OMSetRTV({ outputRTV->getMip(0) }, nullptr);

	RenderAPI::get()->PSSetConstantBuffer({ fadeBuffer }, 1);
	RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);
	RenderAPI::get()->PSSetSRV({ texture2D }, 0);

	RenderAPI::get()->BindShader(RenderAPI::fullScreenVS);
	RenderAPI::get()->BindShader(fadePShader);

	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderAPI::get()->DrawQuad();

	return outputRTV;
}

const float& FadeEffect::getFadeSpeed() const
{
	return fadeParam.fadeSpeed;
}

void FadeEffect::setFadeSpeed(const float& speed)
{
	fadeParam.fadeSpeed = speed;

	memcpy(fadeBuffer->map().pData, &fadeParam, sizeof(FadeParam));
	fadeBuffer->unmap();
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
