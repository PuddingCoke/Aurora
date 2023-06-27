#include<Aurora/Effect/SSREffect.h>

SSREffect::SSREffect(GraphicsContext* const ctx, const UINT& width, const UINT& height) :
	EffectBase(ctx, width, height, FMT::RGBA16UN),
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
	ctx->CSSetSRV({ resDepthTexture }, 0);
	ctx->CSSetUAV({ hiZTexture->getMip(0) }, 0);

	ctx->BindShader(hiZInitializeCS);

	ctx->Dispatch(width / 16, height / 9, 1);

	ctx->BindShader(hiZCreateCS);

	for (UINT i = 0; i < hiZMipLevel - 1; i++)
	{
		ctx->CSSetSRV({ hiZTexture->getMip(i) }, 0);
		ctx->CSSetUAV({ hiZTexture->getMip(i + 1) }, 0);

		ctx->Dispatch(((width / 2) >> i) / 16 + 1, ((height / 2) >> i) / 9 + 1, 1);
	}

	ctx->OMSetBlendState(nullptr);
	ctx->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->ClearRTV(outputRTV->getMip(0), DirectX::Colors::Black);
	ctx->OMSetRTV({ outputRTV->getMip(0) }, nullptr);
	ctx->RSSetViewport(width, height);
	ctx->PSSetSRV({ gPosition,gNormal,hiZTexture }, 0);
	ctx->PSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 1);
	ctx->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::pointClampSampler }, 0);

	ctx->BindShader(Shader::fullScreenVS);
	ctx->BindShader(hiZProcessPS);

	ctx->DrawQuad();

	return outputRTV;
}

void SSREffect::compileShaders()
{
	hiZCreateCS = new Shader(g_HiZCreateCSBytes, sizeof(g_HiZCreateCSBytes), ShaderType::Compute);
	hiZInitializeCS = new Shader(g_HiZInitializeCSBytes, sizeof(g_HiZInitializeCSBytes), ShaderType::Compute);
	hiZProcessPS = new Shader(g_HiZProcessPSBytes, sizeof(g_HiZProcessPSBytes), ShaderType::Pixel);
}
