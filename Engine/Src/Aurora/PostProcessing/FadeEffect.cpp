#include<Aurora/PostProcessing/FadeEffect.h>

FadeEffect::FadeEffect(const unsigned int& width, const unsigned int& height) :
	EffectBase(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT), fadeParam{ 3.f,0.f,0.f,0.f },
	fadeBuffer(new Buffer(sizeof(FadeParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE))
{
	compileShaders();

	setFadeSpeed(3.f);
}

ShaderResourceView* FadeEffect::process(ShaderResourceView* const texture2D) const
{
	RenderAPI::get()->OMSetBlendState(nullptr);

	outputRTV->clearRTV(DirectX::Colors::Black);
	RenderAPI::get()->OMSetRTV({ outputRTV }, nullptr);

	RenderAPI::get()->PSSetBuffer({ fadeBuffer }, 1);
	RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);
	RenderAPI::get()->PSSetSRV({ texture2D }, 0);

	RenderAPI::fullScreenVS->use();
	fadePShader->use();

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

	memcpy(fadeBuffer->map(0).pData, &fadeParam, sizeof(FadeParam));
	fadeBuffer->unmap(0);
}

FadeEffect::~FadeEffect()
{
	delete fadePShader;
}

void FadeEffect::compileShaders()
{
	fadePShader = new Shader(g_FadePSBytes, sizeof(g_FadePSBytes), ShaderType::Pixel);
}
