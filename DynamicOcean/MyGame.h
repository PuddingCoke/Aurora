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

	DepthStencilView* depthView;

	ComPtr<ID3D11RasterizerState> wireframeRS;

	bool showWireframe=false;

	MyGame() :
		camera({ -240,200,0 }, { 1,-1,0 }, { 0,1,0 }, 100, 3),
		ocean(1024, 256, { 16.f,0.f }, 0.000007f),
		depthView(DepthStencilView::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, true))
	{
		{
			D3D11_RASTERIZER_DESC rsDesc = {};
			rsDesc.CullMode = D3D11_CULL_BACK;
			rsDesc.FillMode = D3D11_FILL_WIREFRAME;

			Renderer::device->CreateRasterizerState(&rsDesc, wireframeRS.ReleaseAndGetAddressOf());

			Keyboard::addKeyDownEvent(Keyboard::K, [this]()
				{
					showWireframe = !showWireframe;
				});
		}

		camera.registerEvent();

		Camera::setProj(Math::pi / 5.f, Graphics::getAspectRatio(), 0.1f, 1000.f);
	}

	~MyGame()
	{
		delete depthView;
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

		ocean.update();

		if (showWireframe)
		{
			Renderer::context->RSSetState(wireframeRS.Get());
		}
		else
		{
			Renderer::context->RSSetState(States::rasterCullBack.Get());
		}

		ocean.render();
	}


};