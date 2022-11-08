﻿#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A3D/OrbitCamera.h>

#include"Scene.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Scene scene;

	OrbitCamera camera;

	DepthStencilView* depthView;

	Buffer* lightBuffer;

	struct Light
	{
		DirectX::XMFLOAT4 lightPos;
		DirectX::XMFLOAT4 lightColor;
	}light;

	MyGame() :
		scene("D:/Assets/PBRRendering/DNA.obj"),
		depthView(new DepthStencilView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, true)),
		camera({ 2,0,0 }, { 0,1,0 }),
		lightBuffer(new Buffer(sizeof(Light), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE))
	{
		light.lightColor = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);

		camera.registerEvent();
	}

	~MyGame()
	{
	}

	void update(const float& dt) override
	{
		DirectX::XMFLOAT4 lightPos = Camera::getEye();
		DirectX::XMStoreFloat4(&lightPos, DirectX::XMVectorScale(DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&lightPos)), 1.15f));
		light.lightPos = lightPos;

		memcpy(lightBuffer->map(0).pData, &light, sizeof(Light));
		lightBuffer->unmap(0);

	}

	void render()
	{
		depthView->clear(D3D11_CLEAR_DEPTH);
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::CadetBlue);
		RenderAPI::get()->OMSetDefRTV(depthView);

		RenderAPI::get()->PSSetBuffer({ lightBuffer }, 3);

		scene.draw();
	}


};