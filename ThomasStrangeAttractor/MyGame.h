#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/PostProcessing/BloomEffect.h>
#include<Aurora/A3D/DepthStencilView.h>
#include<Aurora/A3D/OrbitCamera.h>

#include"StrangeAttractor.h"
#include"DualKawaseBlur.h"

//关于这种吸引子
//https://gereshes.com/2020/01/13/attracted-to-attractors/
class MyGame :public Game
{
public:

	StrangeAttractor attractor;

	DepthStencilView* depthStencilView;

	RenderTexture* renderTexture;

	BloomEffect bloomEffect;

	OrbitCamera camera;

	DualKawaseBlur kawaseBlur;

	bool rotating = true;

	bool useKawasBlur = true;

	MyGame() :
		attractor(1000000),
		renderTexture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Transparent)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		depthStencilView(DepthStencilView::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT)),
		camera({ 4,4,-11 }, { -1,-1,-1 }),
		kawaseBlur(Graphics::getWidth(), Graphics::getHeight())
	{
		camera.registerEvent();

		Keyboard::addKeyDownEvent(Keyboard::K, [this]() {
			rotating = !rotating;
			});

		Keyboard::addKeyDownEvent(Keyboard::F, [this]()
			{
				useKawasBlur = !useKawasBlur;
			});
	}

	~MyGame()
	{
		delete renderTexture;
		delete depthStencilView;
	}

	void update(const float& dt) override
	{
		if (rotating)
		{
			camera.rotateX(dt);
		}

		attractor.update(dt);
	}

	void render() override
	{
		Renderer::context->ClearDepthStencilView(depthStencilView->get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		renderTexture->clearRTV(DirectX::Colors::Transparent);
		renderTexture->setRTV(depthStencilView->get());

		attractor.render();

		ID3D11RenderTargetView* view = nullptr;
		Renderer::context->OMSetRenderTargets(1, &view, nullptr);

		ID3D11ShaderResourceView* resourceViews[2] = { nullptr,nullptr };
		Renderer::context->PSSetShaderResources(0, 2, resourceViews);

		Texture2D* texture;

		if (useKawasBlur)
		{
			texture = kawaseBlur.process(renderTexture->getTexture());
		}
		else
		{
			texture = bloomEffect.process(renderTexture->getTexture());
		}

		Renderer::setDefRTV();
		Renderer::clearDefRTV(DirectX::Colors::Black);

		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Renderer::context->PSSetSamplers(0, 1, StateCommon::defLinearSampler.GetAddressOf());
		texture->setSRV(0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		Renderer::context->Draw(3, 0);

		Renderer::context->OMSetRenderTargets(1, &view, nullptr);
		Renderer::context->PSSetShaderResources(0, 2, resourceViews);

	}
};