﻿#include<Aurora/Effect/BloomEffect.h>

BloomEffect::BloomEffect(GraphicsContext* const ctx, const unsigned int& width, const unsigned int& height) :
	EffectBase(ctx, width, height, FMT::RGBA16F), bloomWidth(width), bloomHeight(height), bloomParam{},
	filterTexture(new RenderTexture(width, height, FMT::RGBA16F, DirectX::Colors::Black)),
	lensDirtTexture(new ResourceTexture(Utils::getRootFolder() + "bloom_dirt_mask.png"))
{
	compileShaders();

	{
		//const float sigma[blurSteps] = { 0.55f,0.84f,1.15f,1.9f,3.3f };
		const float sigma[blurSteps] = { 0.44f,0.57f,0.8f,1.32f,3.3f };

		for (unsigned int i = 0; i < blurSteps; i++)
		{
			resolutions[i] = DirectX::XMUINT2(width >> (i + 1), height >> (i + 1));

			swapTexture[i] = new SwapTexture<RenderComputeTexture>([=] {return new RenderComputeTexture(resolutions[i].x, resolutions[i].y, FMT::RGBA16F, DirectX::Colors::Black); });

			blurParam[i].texelSize = DirectX::XMFLOAT2(1.f / resolutions[i].x, 1.f / resolutions[i].y);
			blurParam[i].iteration = iteration[i];
			blurParam[i].sigma = sigma[i];

			blurParamBuffer[i] = new ConstantBuffer(sizeof(BlurParam), D3D11_USAGE_DYNAMIC);

			updateCurve(i);
		}
	}

	bloomParamBuffer = new ConstantBuffer(sizeof(BloomParam), D3D11_USAGE_DYNAMIC);

	bloomParam.exposure = 1.f;
	bloomParam.gamma = 1.f;
	bloomParam.threshold = 1.f;
	bloomParam.intensity = 1.f;
	bloomParam.softThreshold = 1.f;

	applyChange();
}

BloomEffect::~BloomEffect()
{
	for (unsigned int i = 0; i < blurSteps; i++)
	{
		delete swapTexture[i];
		delete blurParamBuffer[i];
	}

	delete bloomParamBuffer;

	delete filterTexture;

	delete bloomFilter;
	delete bloomFinal;

	delete lensDirtTexture;

	delete bloomHBlur;
	delete bloomVBlur;
	delete bloomDownSample;
	delete bloomKarisAverage;
}

ShaderResourceView* BloomEffect::process(ShaderResourceView* const texture2D) const
{
	ctx->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->CSSetSampler({ States::linearClampSampler }, 0);
	ctx->PSSetConstantBuffer({ bloomParamBuffer }, 1);
	ctx->CSSetConstantBuffer({ bloomParamBuffer }, 1);
	ctx->GSUnbindShader();
	ctx->HSUnbindShader();
	ctx->DSUnbindShader();
	ctx->BindShader(Shader::fullScreenVS);

	ctx->OMSetBlendState(nullptr);

	ctx->PSSetSampler({ States::pointClampSampler }, 0);

	ctx->BindShader(bloomFilter);
	ctx->RSSetViewport(bloomWidth, bloomHeight);
	ctx->OMSetRTV({ filterTexture->getMip(0) }, nullptr);
	ctx->PSSetSRV({ texture2D }, 0);
	ctx->DrawQuad();

	ctx->PSSetSampler({ States::linearClampSampler }, 0);

	ctx->BindShader(bloomKarisAverage);
	ctx->RSSetViewport(resolutions[0].x, resolutions[0].y);
	ctx->OMSetRTV({ swapTexture[0]->write()->getMip(0) }, nullptr);
	ctx->PSSetSRV({ filterTexture }, 0);
	ctx->DrawQuad();
	swapTexture[0]->swap();

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		ctx->RSSetViewport(resolutions[i + 1].x, resolutions[i + 1].y);
		ctx->OMSetRTV({ swapTexture[i + 1]->write()->getMip(0) }, nullptr);
		ctx->PSSetSRV({ swapTexture[i]->read() }, 0);
		ctx->DrawQuad();
		swapTexture[i + 1]->swap();
	}

	ctx->OMSetBlendState(States::addtiveBlend);

	ctx->CSSetConstantBuffer({ blurParamBuffer[blurSteps - 1] }, 1);

	ctx->BindShader(bloomHBlur);
	ctx->CSSetUAV({ swapTexture[blurSteps - 1]->write()->getMip(0) }, 0);
	ctx->CSSetSRV({ swapTexture[blurSteps - 1]->read() }, 0);
	swapTexture[blurSteps - 1]->swap();
	ctx->Dispatch(resolutions[blurSteps - 1].x / workGroupSize.x, resolutions[blurSteps - 1].y / workGroupSize.y + 1, 1);

	ctx->BindShader(bloomVBlur);
	ctx->CSSetUAV({ swapTexture[blurSteps - 1]->write()->getMip(0) }, 0);
	ctx->CSSetSRV({ swapTexture[blurSteps - 1]->read() }, 0);
	swapTexture[blurSteps - 1]->swap();
	ctx->Dispatch(resolutions[blurSteps - 1].x / workGroupSize.x, resolutions[blurSteps - 1].y / workGroupSize.y + 1, 1);

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		ctx->CSSetConstantBuffer({ blurParamBuffer[blurSteps - 2 - i] }, 1);

		ctx->BindShader(bloomHBlur);
		ctx->CSSetUAV({ swapTexture[blurSteps - 2 - i]->write()->getMip(0) }, 0);
		ctx->CSSetSRV({ swapTexture[blurSteps - 2 - i]->read() }, 0);
		ctx->Dispatch(resolutions[blurSteps - 2 - i].x / workGroupSize.x, resolutions[blurSteps - 2 - i].y / workGroupSize.y + 1, 1);
		swapTexture[blurSteps - 2 - i]->swap();

		ctx->BindShader(bloomVBlur);
		ctx->CSSetUAV({ swapTexture[blurSteps - 2 - i]->write()->getMip(0) }, 0);
		ctx->CSSetSRV({ swapTexture[blurSteps - 2 - i]->read() }, 0);
		ctx->Dispatch(resolutions[blurSteps - 2 - i].x / workGroupSize.x, resolutions[blurSteps - 2 - i].y / workGroupSize.y + 1, 1);

		ctx->BindShader(Shader::fullScreenPS);
		ctx->RSSetViewport(resolutions[blurSteps - 2 - i].x, resolutions[blurSteps - 2 - i].y);
		ctx->OMSetRTV({ swapTexture[blurSteps - 2 - i]->write()->getMip(0) }, nullptr);
		ctx->PSSetSRV({ swapTexture[blurSteps - 1 - i]->read() }, 0);
		ctx->DrawQuad();
		swapTexture[blurSteps - 2 - i]->swap();
	}

	ctx->BindShader(bloomFinal);
	ctx->ClearRTV(outputRTV->getMip(0), DirectX::Colors::Black);
	ctx->RSSetViewport(bloomWidth, bloomHeight);
	ctx->OMSetRTV({ outputRTV->getMip(0) }, nullptr);
	ctx->PSSetSRV({ texture2D,swapTexture[0]->read(),lensDirtTexture }, 0);
	ctx->DrawQuad();

	return outputRTV;
}

void BloomEffect::setExposure(const float& exposure)
{
	bloomParam.exposure = exposure;
}

void BloomEffect::setGamma(const float& gamma)
{
	bloomParam.gamma = gamma;
}

void BloomEffect::setThreshold(const float& threshold)
{
	bloomParam.threshold = threshold;
}

void BloomEffect::setIntensity(const float& intensity)
{
	bloomParam.intensity = intensity;
}

void BloomEffect::setSoftThreshold(const float& softThreshold)
{
	bloomParam.softThreshold = softThreshold;
}

void BloomEffect::imGUIEffectModifier()
{
	ImGui::Text("BloomEffect Modifier");
	ImGui::SliderFloat("Gaussain Curve0", &blurParam[0].sigma, 0.4f, 4.f);
	ImGui::SliderFloat("Gaussain Curve1", &blurParam[1].sigma, 0.4f, 4.f);
	ImGui::SliderFloat("Gaussain Curve2", &blurParam[2].sigma, 0.4f, 4.f);
	ImGui::SliderFloat("Gaussain Curve3", &blurParam[3].sigma, 0.4f, 4.f);
	ImGui::SliderFloat("Gaussain Curve4", &blurParam[4].sigma, 0.4f, 4.f);
	ImGui::SliderFloat("Exposure", &bloomParam.exposure, 0.0f, 4.f);
	ImGui::SliderFloat("Gamma", &bloomParam.gamma, 0.0f, 4.f);
	ImGui::SliderFloat("Threshold", &bloomParam.threshold, 0.0f, 1.f);
	ImGui::SliderFloat("SoftThreshold", &bloomParam.softThreshold, 0.0f, 1.f);
	ImGui::SliderFloat("Intensity", &bloomParam.intensity, 0.0f, 4.f);


	updateCurve(0);
	updateCurve(1);
	updateCurve(2);
	updateCurve(3);
	updateCurve(4);

	applyChange();
}

void BloomEffect::updateCurve(const unsigned int& index)
{
	blurParam[index].weight[0] = Math::gauss(blurParam[index].sigma, 0.f);

	for (unsigned int i = 1; i < (iteration[index] - 1) * 2 + 1; i += 2)
	{
		const float g1 = Math::gauss(blurParam[index].sigma, (float)i);
		const float g2 = Math::gauss(blurParam[index].sigma, (float)(i + 1));
		blurParam[index].weight[(i + 1) / 2] = g1 + g2;
		blurParam[index].offset[(i + 1) / 2] = (g1 * i + g2 * (i + 1)) / (g1 + g2);
	}

	BufferUpdate::pushBufferUpdateParam(blurParamBuffer[index], &blurParam[index], sizeof(BlurParam));
}

const float& BloomEffect::getExposure() const
{
	return bloomParam.exposure;
}

const float& BloomEffect::getGamma() const
{
	return bloomParam.gamma;
}

const float& BloomEffect::getThreshold() const
{
	return bloomParam.threshold;
}

const float& BloomEffect::getIntensity() const
{
	return bloomParam.intensity;
}

void BloomEffect::applyChange() const
{
	BufferUpdate::pushBufferUpdateParam(bloomParamBuffer, &bloomParam, sizeof(BloomParam));
}

void BloomEffect::compileShaders()
{
	bloomFilter = new Shader(g_BloomFilterPSBytes, sizeof(g_BloomFilterPSBytes), ShaderType::Pixel);
	bloomFinal = new Shader(g_BloomFinalPSBytes, sizeof(g_BloomFinalPSBytes), ShaderType::Pixel);
	bloomVBlur = new Shader(g_BloomVBlurCSBytes, sizeof(g_BloomVBlurCSBytes), ShaderType::Compute);
	bloomHBlur = new Shader(g_BloomHBlurCSBytes, sizeof(g_BloomHBlurCSBytes), ShaderType::Compute);
	bloomDownSample = new Shader(g_BloomDownSamplePSBytes, sizeof(g_BloomDownSamplePSBytes), ShaderType::Pixel);
	bloomKarisAverage = new Shader(g_BloomKarisAveragePSBytes, sizeof(g_BloomKarisAveragePSBytes), ShaderType::Pixel);
}
