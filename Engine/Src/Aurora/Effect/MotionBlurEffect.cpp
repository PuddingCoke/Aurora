#include<Aurora/Effect/MotionBlurEffect.h>

MotionBlurEffect::MotionBlurEffect(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& format) :
	EffectBase(width, height, format)
{
	compileShaders();
}

MotionBlurEffect::~MotionBlurEffect()
{
	delete motionBlurPS;
}

ShaderResourceView* MotionBlurEffect::process(ShaderResourceView* const gPosition, ShaderResourceView* const colorTexture)
{
	RenderAPI::get()->OMSetBlendState(nullptr);
	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	RenderAPI::get()->OMSetRTV({ outputRTV->getRTVMip(0) }, nullptr);

	RenderAPI::get()->PSSetSRV({ gPosition,colorTexture }, 0);
	RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);
	RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

	RenderAPI::fullScreenVS->use();
	motionBlurPS->use();

	RenderAPI::get()->DrawQuad();

	return outputRTV;
}

void MotionBlurEffect::compileShaders()
{
	motionBlurPS = new Shader(g_MotionBlurPSBytes, sizeof(g_MotionBlurPSBytes), ShaderType::Pixel);
}
