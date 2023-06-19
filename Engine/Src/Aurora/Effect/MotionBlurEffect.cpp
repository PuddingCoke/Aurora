#include<Aurora/Effect/MotionBlurEffect.h>

MotionBlurEffect::MotionBlurEffect(const unsigned int& width, const unsigned int& height, const FMT& format) :
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
	ImCtx::get()->OMSetBlendState(nullptr);
	ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ImCtx::get()->OMSetRTV({ outputRTV->getMip(0) }, nullptr);

	ImCtx::get()->PSSetSRV({ gPosition,colorTexture }, 0);
	ImCtx::get()->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);
	ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

	ImCtx::get()->BindShader(ImCtx::fullScreenVS);
	ImCtx::get()->BindShader(motionBlurPS);

	ImCtx::get()->DrawQuad();

	return outputRTV;
}

void MotionBlurEffect::compileShaders()
{
	motionBlurPS = new Shader(g_MotionBlurPSBytes, sizeof(g_MotionBlurPSBytes), ShaderType::Pixel);
}
