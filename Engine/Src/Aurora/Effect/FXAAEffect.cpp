#include<Aurora/Effect/FXAAEffect.h>

FXAAEffect::FXAAEffect(GraphicsContext* const ctx, const UINT& width, const UINT& height) :
	EffectBase(ctx, width, height, FMT::RGBA8UN), fxaaParam{ 1.0f,0.75f,0.166f,0.0633f },
	colorWithLuma(new RenderTexture(width, height, FMT::RGBA8UN))
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
	ctx->OMSetBlendState(nullptr);
	ctx->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->BindShader(Shader::fullScreenVS);

	ctx->ClearRTV(colorWithLuma->getMip(0), DirectX::Colors::Black);
	ctx->OMSetRTV({ colorWithLuma->getMip(0) }, nullptr);
	ctx->PSSetSRV({ texture2D }, 0);
	ctx->PSSetSampler({ States::pointClampSampler }, 0);

	ctx->BindShader(colorToLuma);

	ctx->DrawQuad();

	ctx->ClearRTV(outputRTV->getMip(0), DirectX::Colors::Black);
	ctx->OMSetRTV({ outputRTV->getMip(0) }, nullptr);
	ctx->PSSetSRV({ colorWithLuma }, 0);
	ctx->PSSetSampler({ States::linearClampSampler }, 0);
	ctx->PSSetConstantBuffer({ fxaaParamBuffer }, 1);

	ctx->BindShader(fxaaPS);

	ctx->DrawQuad();

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
	BufferUpdate::pushBufferUpdateParam(fxaaParamBuffer, &fxaaParam, sizeof(FXAAParam));
}

void FXAAEffect::compileShaders()
{
	colorToLuma = new Shader(g_ColorToLumaBytes, sizeof(g_ColorToLumaBytes), ShaderType::Pixel);
	fxaaPS = new Shader(g_FXAABytes, sizeof(g_FXAABytes), ShaderType::Pixel);
}
