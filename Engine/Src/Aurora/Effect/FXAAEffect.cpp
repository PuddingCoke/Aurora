#include<Aurora/Effect/FXAAEffect.h>

FXAAEffect::FXAAEffect(const UINT& width, const UINT& height) :
	EffectBase(width, height, DXGI_FORMAT_R8G8B8A8_UNORM), fxaaParam{ 1.0f,0.75f,0.166f,0.0633f },
	colorWithLuma(new RenderTexture(width, height, DXGI_FORMAT_R8G8B8A8_UNORM))
{
	compileShaders();

	fxaaParamBuffer = new ConstantBuffer(sizeof(FXAAParam), D3D11_USAGE_DYNAMIC, &fxaaParam);
}

FXAAEffect::~FXAAEffect()
{
	delete colorWithLuma;
	delete fxaaParamBuffer;
	delete colorToLuma;
	delete fxaaPS;
}

ShaderResourceView* FXAAEffect::process(ShaderResourceView* const texture2D) const
{
	RenderAPI::get()->OMSetBlendState(nullptr);
	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderAPI::fullScreenVS->use();

	colorWithLuma->clearRTV(DirectX::Colors::Black, 0);
	RenderAPI::get()->OMSetRTV({ colorWithLuma->getRTVMip(0) }, nullptr);
	RenderAPI::get()->PSSetSRV({ texture2D }, 0);
	RenderAPI::get()->PSSetSampler({ States::pointClampSampler }, 0);

	colorToLuma->use();

	RenderAPI::get()->DrawQuad();

	outputRTV->clearRTV(DirectX::Colors::Black, 0);
	RenderAPI::get()->OMSetRTV({ outputRTV->getRTVMip(0) }, nullptr);
	RenderAPI::get()->PSSetSRV({ colorWithLuma }, 0);
	RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);
	RenderAPI::get()->PSSetConstantBuffer({ fxaaParamBuffer }, 1);

	fxaaPS->use();

	RenderAPI::get()->DrawQuad();

	return outputRTV;
}

void FXAAEffect::setFXAAQualitySubpix(const float& fxaaQualitySubpix)
{
	fxaaParam.fxaaQualitySubpix = fxaaQualitySubpix;
}

void FXAAEffect::setFXAAQualityEdgeThreshold(const float& fxaaQualityEdgeThreshold)
{
	fxaaParam.fxaaQualityEdgeThreshold = fxaaQualityEdgeThreshold;
}

void FXAAEffect::setFXAAQualityEdgeThresholdMin(const float& fxaaQualityEdgeThresholdMin)
{
	fxaaParam.fxaaQualityEdgeThresholdMin = fxaaQualityEdgeThresholdMin;
}

void FXAAEffect::imGUIEffectModifier()
{
	ImGui::Text("FXAAEffect Modifier");
	ImGui::SliderFloat("FXAAQualitySubpix", &fxaaParam.fxaaQualitySubpix, 0.0f, 1.f);
	ImGui::SliderFloat("FXAAQualityEdgeThreshold", &fxaaParam.fxaaQualityEdgeThreshold, 0.0f, 1.f);
	ImGui::SliderFloat("FXAAQualityEdgeThresholdMin", &fxaaParam.fxaaQualityEdgeThresholdMin, 0.0f, 1.f);

	applyChange();
}

const float& FXAAEffect::getFXAAQualitySubpix() const
{
	return fxaaParam.fxaaQualitySubpix;
}

const float& FXAAEffect::getFXAAQualityEdgeThreshold() const
{
	return fxaaParam.fxaaQualityEdgeThreshold;
}

const float& FXAAEffect::getFXAAQualityEdgeThresholdMin() const
{
	return fxaaParam.fxaaQualityEdgeThresholdMin;
}

void FXAAEffect::applyChange() const
{
	memcpy(fxaaParamBuffer->map().pData, &fxaaParam, sizeof(FXAAParam));
	fxaaParamBuffer->unmap();
}

void FXAAEffect::compileShaders()
{
	colorToLuma = new Shader(g_ColorToLumaBytes, sizeof(g_ColorToLumaBytes), ShaderType::Pixel);
	fxaaPS = new Shader(g_FXAABytes, sizeof(g_FXAABytes), ShaderType::Pixel);
}
