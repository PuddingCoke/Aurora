#include<Aurora/Effect/SSREffect.h>

SSREffect::SSREffect(const UINT& width, const UINT& height) :
	EffectBase(width, height, FMT::RGBA16UN),
	hiZTexture(new ComputeTexture(width, height, FMT::R32F, FMT::R32F, FMT::R32F, hiZMipLevel, 1))
{
	compileShaders();
}

SSREffect::~SSREffect()
{
	delete hiZCreateCS;
	delete hiZInitializeCS;
	delete hiZProcessPS;
	delete hiZTexture;
}

ShaderResourceView* SSREffect::process(ShaderResourceView* resDepthTexture, ShaderResourceView* gPosition, ShaderResourceView* gNormal) const
{
	RenderAPI::get()->CSSetSRV({ resDepthTexture }, 0);
	RenderAPI::get()->CSSetUAV({ hiZTexture->getMip(0) }, 0);

	RenderAPI::get()->BindShader(hiZInitializeCS);

	RenderAPI::get()->Dispatch(width / 16, height / 9, 1);

	RenderAPI::get()->BindShader(hiZCreateCS);

	for (UINT i = 0; i < hiZMipLevel - 1; i++)
	{
		RenderAPI::get()->CSSetSRV({ hiZTexture->getMip(i) }, 0);
		RenderAPI::get()->CSSetUAV({ hiZTexture->getMip(i + 1) }, 0);

		RenderAPI::get()->Dispatch(((width / 2) >> i) / 16 + 1, ((height / 2) >> i) / 9 + 1, 1);
	}

	outputRTV->clearRTV(DirectX::Colors::Black, 0);
	RenderAPI::get()->OMSetRTV({ outputRTV->getMip(0) }, nullptr);
	RenderAPI::get()->PSSetSRV({ gPosition,gNormal,hiZTexture }, 0);
	RenderAPI::get()->PSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 1);
	RenderAPI::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::pointClampSampler }, 0);

	RenderAPI::get()->BindShader(RenderAPI::fullScreenVS);
	RenderAPI::get()->BindShader(hiZProcessPS);

	RenderAPI::get()->DrawQuad();

	return outputRTV;
}

void SSREffect::compileShaders()
{
	hiZCreateCS = new Shader(g_HiZCreateCSBytes, sizeof(g_HiZCreateCSBytes), ShaderType::Compute);
	hiZInitializeCS = new Shader(g_HiZInitializeCSBytes, sizeof(g_HiZInitializeCSBytes), ShaderType::Compute);
	hiZProcessPS = new Shader(g_HiZProcessPSBytes, sizeof(g_HiZProcessPSBytes), ShaderType::Pixel);
}
