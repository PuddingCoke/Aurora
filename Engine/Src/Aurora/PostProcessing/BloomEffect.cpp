#include<Aurora/PostProcessing/BloomEffect.h>

BloomEffect::BloomEffect(const unsigned int& width, const unsigned int& height) :
	EffectBase(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT), bloomWidth(width), bloomHeight(height), bloomParam{},
	originTexture(new RenderTexture(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
	bloomTexture(new RenderTexture(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
	lensDirtTexture(new ResourceTexture(Utils::getRootFolder() + "bloom_dirt_mask.png"))
{
	compileShaders();

	{

		//const float sigma[blurSteps] = { 0.55f,0.84f,1.15f,1.9f,3.3f };
		const float sigma[blurSteps] = { 0.44f,0.57f,0.8f,1.32f,3.3f };

		for (unsigned int i = 0; i < blurSteps; i++)
		{
			resolutions[i] = DirectX::XMUINT2(width >> (i + 1), height >> (i + 1));

			rcTextures[i * 2u] = new RCTexture(resolutions[i].x, resolutions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black);
			rcTextures[i * 2u + 1u] = new RCTexture(resolutions[i].x, resolutions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black);

			blurParam[i].texelSize = DirectX::XMFLOAT2(1.f / resolutions[i].x, 1.f / resolutions[i].y);
			blurParam[i].iteration = iteration[i];
			blurParam[i].sigma = sigma[i];

			blurParamBuffer[i] = new StructuredBuffer(sizeof(BlurParam), sizeof(BlurParam), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

			updateCurve(i);
		}
	}

	bloomParamBuffer = new Buffer(sizeof(BloomParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	bloomParam.exposure = 0.36f;
	bloomParam.gamma = 1.0f;
	bloomParam.threshold = 1.0f;
	bloomParam.intensity = 1.0f;

	applyChange();
}

BloomEffect::~BloomEffect()
{
	for (unsigned int i = 0; i < blurSteps; i++)
	{
		delete rcTextures[i * 2u];
		delete rcTextures[i * 2u + 1u];
		delete blurParamBuffer[i];
	}

	delete bloomParamBuffer;

	delete originTexture;
	delete bloomTexture;

	delete bloomExtract;
	delete bloomFinal;

	delete lensDirtTexture;

	delete bloomHBlur;
	delete bloomVBlur;
	delete bloomDownSample;
}

ShaderResourceView* BloomEffect::process(ShaderResourceView* const texture2D) const
{
	RenderAPI::get()->OMSetBlendState(nullptr);
	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);
	RenderAPI::get()->PSSetConstantBuffer({ bloomParamBuffer }, 1);
	RenderAPI::get()->CSSetConstantBuffer({ bloomParamBuffer }, 1);
	RenderAPI::get()->HSSetShader(nullptr);
	RenderAPI::get()->DSSetShader(nullptr);
	RenderAPI::fullScreenVS->use();

	RenderAPI::get()->PSSetSampler({ States::pointClampSampler }, 0);

	bloomExtract->use();
	RenderAPI::get()->RSSetViewport(bloomWidth, bloomHeight);
	RenderAPI::get()->OMSetRTV({ originTexture,bloomTexture }, nullptr);
	RenderAPI::get()->PSSetSRV({ texture2D }, 0);
	RenderAPI::get()->DrawQuad();

	RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

	bloomDownSample->use();
	RenderAPI::get()->RSSetViewport(resolutions[0].x, resolutions[0].y);
	RenderAPI::get()->OMSetRTV({ rcTextures[0] }, nullptr);
	RenderAPI::get()->PSSetSRV({ bloomTexture }, 0);
	RenderAPI::get()->DrawQuad();

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		RenderAPI::get()->RSSetViewport(resolutions[i + 1].x, resolutions[i + 1].y);
		RenderAPI::get()->OMSetRTV({ rcTextures[(i + 1) * 2] }, nullptr);
		RenderAPI::get()->PSSetSRV({ rcTextures[i * 2] }, 0);
		RenderAPI::get()->DrawQuad();
	}

	RenderAPI::get()->OMSetBlendState(States::addtiveBlend);

	RenderAPI::get()->CSSetSRV({ blurParamBuffer[blurSteps - 1] }, 1);

	bloomHBlur->use();
	RenderAPI::get()->CSSetUAV({ rcTextures[(blurSteps - 1) * 2 + 1] }, 0);
	RenderAPI::get()->CSSetSRV({ rcTextures[(blurSteps - 1) * 2] }, 0);
	RenderAPI::get()->Dispatch(rcTextures[(blurSteps - 1) * 2]->getWidth() / workGroupSize.x, rcTextures[(blurSteps - 1) * 2]->getHeight() / workGroupSize.y + 1, 1);

	bloomVBlur->use();
	RenderAPI::get()->CSSetUAV({ rcTextures[(blurSteps - 1) * 2] }, 0);
	RenderAPI::get()->CSSetSRV({ rcTextures[(blurSteps - 1) * 2 + 1] }, 0);
	RenderAPI::get()->Dispatch(rcTextures[(blurSteps - 1) * 2]->getWidth() / workGroupSize.x, rcTextures[(blurSteps - 1) * 2]->getHeight() / workGroupSize.y + 1, 1);

	for (unsigned int i = 0; i < blurSteps - 1; i++)
	{
		RenderAPI::get()->CSSetSRV({ blurParamBuffer[blurSteps - 2 - i] }, 1);

		bloomHBlur->use();
		RenderAPI::get()->CSSetUAV({ rcTextures[(blurSteps - 2 - i) * 2 + 1] }, 0);
		RenderAPI::get()->CSSetSRV({ rcTextures[(blurSteps - 2 - i) * 2] }, 0);
		RenderAPI::get()->Dispatch(rcTextures[(blurSteps - 2 - i) * 2]->getWidth() / workGroupSize.x, rcTextures[(blurSteps - 2 - i) * 2]->getHeight() / workGroupSize.y + 1, 1);

		bloomVBlur->use();
		RenderAPI::get()->CSSetUAV({ rcTextures[(blurSteps - 2 - i) * 2] }, 0);
		RenderAPI::get()->CSSetSRV({ rcTextures[(blurSteps - 2 - i) * 2 + 1] }, 0);
		RenderAPI::get()->Dispatch(rcTextures[(blurSteps - 2 - i) * 2]->getWidth() / workGroupSize.x, rcTextures[(blurSteps - 2 - i) * 2]->getHeight() / workGroupSize.y + 1, 1);

		RenderAPI::get()->RSSetViewport(resolutions[blurSteps - 2 - i].x, resolutions[blurSteps - 2 - i].y);

		RenderAPI::fullScreenPS->use();
		RenderAPI::get()->OMSetRTV({ rcTextures[(blurSteps - 2 - i) * 2] }, nullptr);
		RenderAPI::get()->PSSetSRV({ rcTextures[(blurSteps - 1 - i) * 2] }, 0);
		RenderAPI::get()->DrawQuad();
	}

	RenderAPI::get()->RSSetViewport(bloomWidth, bloomHeight);
	RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);
	RenderAPI::get()->PSSetSRV({ rcTextures[0] }, 0);
	RenderAPI::get()->DrawQuad();

	bloomFinal->use();
	outputRTV->clearRTV(DirectX::Colors::Black);
	RenderAPI::get()->OMSetRTV({ outputRTV }, nullptr);
	RenderAPI::get()->PSSetSRV({ originTexture,lensDirtTexture }, 0);
	RenderAPI::get()->DrawQuad();

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
	ImGui::SliderFloat("Threshold", &bloomParam.threshold, 0.0f, 4.f);
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

	memcpy(blurParamBuffer[index]->map(0).pData, &blurParam[index], sizeof(BlurParam));
	blurParamBuffer[index]->unmap(0);
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
	memcpy(bloomParamBuffer->map(0).pData, &bloomParam, sizeof(BloomParam));
	bloomParamBuffer->unmap(0);
}

void BloomEffect::compileShaders()
{
	bloomExtract = new Shader(g_BloomExtractPSBytes, sizeof(g_BloomExtractPSBytes), ShaderType::Pixel);
	bloomFinal = new Shader(g_BloomFinalPSBytes, sizeof(g_BloomFinalPSBytes), ShaderType::Pixel);
	bloomVBlur = new Shader(g_BloomVBlurCSBytes, sizeof(g_BloomVBlurCSBytes), ShaderType::Compute);
	bloomHBlur = new Shader(g_BloomHBlurCSBytes, sizeof(g_BloomHBlurCSBytes), ShaderType::Compute);
	bloomDownSample = new Shader(g_BloomDownSamplePSBytes, sizeof(g_BloomDownSamplePSBytes), ShaderType::Pixel);
}
