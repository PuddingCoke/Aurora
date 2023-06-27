#pragma once

#include<Aurora/EngineAPI/RenderPass.h>
#include<Aurora/EngineAPI/ResourceEssential.h>
#include<Aurora/EngineAPI/Graphics.h>
#include<Aurora/EngineAPI/Camera.h>

#include<Aurora/Effect/BloomEffect.h>
#include<Aurora/Effect/FXAAEffect.h>
#include<Aurora/Effect/SSREffect.h>

class PostProcessPass :public RenderPass
{
public:

	PostProcessPass():
		bloomEffect(context, Graphics::getWidth(), Graphics::getHeight()),
		fxaaEffect(context, Graphics::getWidth(), Graphics::getHeight()),
		ssrEffect(context, Graphics::getWidth(), Graphics::getHeight()),
		ssrCombineShader(new Shader(Utils::getRootFolder() + "SSRCombineShader.cso", ShaderType::Pixel)),
		reflectedColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F, DirectX::Colors::Black))
	{
		bloomEffect.setIntensity(0.5f);
		bloomEffect.applyChange();
	}

	~PostProcessPass()
	{
		delete depthTexture;
		delete gPosition;
		delete gNormalSpecular;
		delete originTexture;
		delete reflectedColor;
		delete ssrCombineShader;
	}

	ResourceDepthTexture* depthTexture;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* originTexture;

	RenderTexture* reflectedColor;

	Shader* ssrCombineShader;

	BloomEffect bloomEffect;

	FXAAEffect fxaaEffect;

	SSREffect ssrEffect;

private:

	void recordCommand() override
	{
		context->OMSetBlendState(nullptr);
		context->RSSetState(States::rasterCullBack);
		context->OMSetDepthStencilState(States::defDepthStencilState, 0);

		ShaderResourceView* const uvVisibilitySRV = ssrEffect.process(depthTexture, gPosition, gNormalSpecular);

		context->OMSetRTV({ reflectedColor->getMip(0) }, nullptr);
		context->PSSetSRV({ originTexture,uvVisibilitySRV,gNormalSpecular }, 0);
		context->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler }, 0);

		context->BindShader(Shader::fullScreenVS);
		context->BindShader(ssrCombineShader);

		context->DrawQuad();

		ShaderResourceView* const bloomTextureSRV = bloomEffect.process(reflectedColor);

		ShaderResourceView* const antiAliasedSRV = fxaaEffect.process(bloomTextureSRV);

		context->OMSetBlendState(nullptr);

		context->OMSetDefRTV(nullptr);
		context->PSSetSRV({ antiAliasedSRV }, 0);

		context->BindShader(Shader::fullScreenVS);
		context->BindShader(Shader::fullScreenPS);

		context->DrawQuad();
	}

};