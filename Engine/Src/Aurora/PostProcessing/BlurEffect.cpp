#include <Aurora/PostProcessing/BlurEffect.h>

BlurEffect::BlurEffect(const unsigned int& width, const unsigned int& height) :
	EffectBase(width, height), renderTexture(RenderTexture::create(width / 2, height / 2, DXGI_FORMAT_R8G8B8A8_UNORM))
{
}

BlurEffect::~BlurEffect()
{
	delete renderTexture;
}

Texture2D* BlurEffect::process(Texture2D* const texture2D) const
{
	Graphics::setViewport(width / 2, height / 2);

	renderTexture->setRTV();
	Graphics::context->ClearRenderTargetView(renderTexture->normalTarget.Get(), DirectX::Colors::Transparent);

	Shader::displayPShader->use();
	Shader::displayVShader->use();

	Graphics::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());
	texture2D->setSRV(0);

	Graphics::context->Draw(3, 0);

	Graphics::setViewport(width, height);
	defRenderTexture->setRTV();
	Graphics::context->ClearRenderTargetView(defRenderTexture->normalTarget.Get(), DirectX::Colors::Transparent);
	renderTexture->getTexture()->setSRV(0);

	Graphics::context->Draw(3, 0);

	return defRenderTexture->getTexture();
}

void BlurEffect::compileShaders()
{

}
