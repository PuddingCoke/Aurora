#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/PostProcessing/BloomEffect.h>
#include<Aurora/DepthStencilView.h>
#include<Aurora/A3D/OrbitCamera.h>

#include"StrangeAttractor.h"

//关于这种吸引子
//https://gereshes.com/2020/01/13/attracted-to-attractors/
class MyGame :public Game
{
public:

	StrangeAttractor attractor;

	float theta = 0.f;

	const float radius = 15.f;

	DepthStencilView* depthStencilView;

	RenderTexture* renderTexture;

	BloomEffect bloomEffect;

	OrbitCamera camera;

	MyGame() :
		attractor(1000000),
		renderTexture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Transparent, false)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		depthStencilView(DepthStencilView::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, false)),
		camera({ 4,4,-11 }, {-1,-1,-1})
	{
		camera.registerEvent();
	}

	~MyGame()
	{
		delete renderTexture;
		delete depthStencilView;
	}

	void update(const float& dt) override
	{
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

		Texture2D* const texture = bloomEffect.process(renderTexture->getTexture());

		Renderer::setDefRTV();
		Renderer::clearDefRTV(DirectX::Colors::Black);

		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Renderer::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());
		texture->setSRV(0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		Renderer::context->Draw(3, 0);

		Renderer::context->OMSetRenderTargets(1, &view, nullptr);
		Renderer::context->PSSetShaderResources(0, 2, resourceViews);

	}
};