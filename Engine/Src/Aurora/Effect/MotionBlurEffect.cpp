#include<Aurora/Effect/MotionBlurEffect.h>

MotionBlurEffect::MotionBlurEffect(GraphicsContext* const ctx, const unsigned int& width, const unsigned int& height, const FMT& format) :
	EffectBase(ctx, width, height, format)
{
	compileShaders();
}

MotionBlurEffect::~MotionBlurEffect()
{
	delete motionBlurPS;
}

ShaderResourceView* MotionBlurEffect::process(ShaderResourceView* const gPosition, ShaderResourceView* const colorTexture)
{
	ctx->OMSetBlendState(nullptr);
	ctx->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ctx->ClearRTV(outputRTV->getMip(0), DirectX::Colors::Black);
	ctx->OMSetRTV({ outputRTV->getMip(0) }, nullptr);

	ctx->PSSetSRV({ gPosition,colorTexture }, 0);
	ctx->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);
	ctx->PSSetSampler({ States::linearClampSampler }, 0);

	ctx->BindShader(Shader::fullScreenVS);
	ctx->BindShader(motionBlurPS);

	ctx->DrawQuad();

	return outputRTV;
}

void MotionBlurEffect::compileShaders()
{
	motionBlurPS = new Shader(g_MotionBlurPSBytes, sizeof(g_MotionBlurPSBytes), ShaderType::Pixel);
}
