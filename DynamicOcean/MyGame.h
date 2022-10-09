#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/A3D/DepthStencilView.h>

#include"Ocean.h"

class MyGame :public Game
{
public:

	FPSCamera camera;

	Ocean ocean;

	Shader* debugShader;

	DepthStencilView* depthView;

	ComPtr<ID3D11RasterizerState> wireframeRS;

	MyGame() :
		camera({ 0,100,0 }, { 1,0,0 }, { 0,1,0 }, 1000, 3),
		ocean(1024, 8192, { 5.f,0.f }, 0.0005f),
		depthView(DepthStencilView::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, true)),
		debugShader(Shader::fromFile("DebugShader.hlsl", ShaderType::Pixel))
	{
		{
			D3D11_RASTERIZER_DESC rsDesc = {};
			rsDesc.CullMode = D3D11_CULL_BACK;
			rsDesc.FillMode = D3D11_FILL_WIREFRAME;

			Renderer::device->CreateRasterizerState(&rsDesc, wireframeRS.ReleaseAndGetAddressOf());
		}

		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 0.1f, 10000.f);
	}

	~MyGame()
	{
		delete depthView;
		delete debugShader;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		depthView->clear(D3D11_CLEAR_DEPTH);
		Renderer::clearDefRTV(DirectX::Colors::AliceBlue);
		Renderer::setDefRTV(depthView->get());

		ocean.calcDisplacement();

		//Renderer::context->RSSetState(wireframeRS.Get());

		ocean.render();
	}


};