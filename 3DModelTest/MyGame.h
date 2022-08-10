#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/A3D/Model.h>
#include<Aurora/DepthStencilView.h>

class MyGame :public Game
{
public:

	Model* models;

	unsigned int numModels;

	Shader* modelVShader;

	Shader* modelPShader;

	ComPtr<ID3D11InputLayout> inputLayout;

	DepthStencilView* depthStencilView;

	static constexpr float focusY = 0.60f;

	static constexpr DirectX::XMVECTORF32 up = { 0.f, 0.f, 1.f , 0.f };

	static constexpr DirectX::XMVECTORF32 focusPoint = { 0.f,0.f,focusY,1.f };

	static constexpr DirectX::XMFLOAT3 eyeOrigin = DirectX::XMFLOAT3(0.f, 0.f, focusY);

	float theta = 0.f;

	float theta2 = 0.25f;

	float curRadius = 1.5f;

	float targetRadius = 1.5f;

	struct LightInfo
	{
		DirectX::XMFLOAT3 lightPos;
		float v0;
		DirectX::XMFLOAT3 viewPos;
		float v1;
	} lightInfo{};

	ComPtr<ID3D11Buffer> lightBuffer;

	Texture2D* envTexture;

	Shader* tonemapShader;

	MyGame() :
		models(Model::create("DNA.obj", numModels)),
		modelVShader(Shader::fromFile("ModelVShader.hlsl", ShaderType::Vertex)),
		modelPShader(Shader::fromFile("ModelPShader.hlsl", ShaderType::Pixel)),
		tonemapShader(Shader::fromFile("TonemapPShader.hlsl",ShaderType::Pixel)),
		depthStencilView(DepthStencilView::create(DXGI_FORMAT_D32_FLOAT)),
		envTexture(Texture2D::create("Tufts-Parking-Lot_Ref.hdr"))
	{
		{
			D3D11_INPUT_ELEMENT_DESC layout[3] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			Graphics::device->CreateInputLayout(layout, 3u, modelVShader->shaderBlob->GetBufferPointer(), modelVShader->shaderBlob->GetBufferSize(), inputLayout.ReleaseAndGetAddressOf());
		}

		{
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(LightInfo);
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

			Graphics::device->CreateBuffer(&bd, nullptr, lightBuffer.ReleaseAndGetAddressOf());
		}

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					theta += Mouse::getDX() * 0.02f;
					theta2 -= Mouse::getDY() * 0.02f;
					theta2 = Math::clamp(theta2, -Math::half_pi + 0.1f, Math::half_pi - 0.1f);
				}
			});

		Mouse::addScrollEvent([this]()
			{
				targetRadius -= Mouse::getWheelDelta() * 0.5f;
				if (targetRadius < 0.1f)
				{
					targetRadius = 0.1f;
				}
			});
	}

	~MyGame()
	{
		delete[] models;
		delete modelVShader;
		delete modelPShader;
		delete depthStencilView;
		delete envTexture;
		delete tonemapShader;
	}

	void update(const float& dt) override
	{
		if ((curRadius - targetRadius) * (curRadius - targetRadius) > 0.0001f)
		{
			curRadius = Math::lerp(curRadius, targetRadius, 10.f * Graphics::getDeltaTime());
		}

		DirectX::XMFLOAT4 eyeRotated = DirectX::XMFLOAT4(curRadius * cosf(theta2) * cosf(theta) + eyeOrigin.x, curRadius * cosf(theta2) * sinf(theta) + eyeOrigin.y, curRadius * sinf(theta2) + eyeOrigin.z, 1.f);

		Graphics::setView(DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&eyeRotated), focusPoint, up)));

		const float lightRadius = 1.15f;

		lightInfo.lightPos = DirectX::XMFLOAT3(lightRadius * cosf(theta2) * cosf(theta) + eyeOrigin.x, lightRadius * cosf(theta2) * sinf(theta) + eyeOrigin.y, lightRadius * sinf(theta2) + eyeOrigin.z);
		lightInfo.viewPos = DirectX::XMFLOAT3(eyeRotated.x, eyeRotated.y, eyeRotated.z);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		Graphics::context->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &lightInfo, sizeof(LightInfo));
		Graphics::context->Unmap(lightBuffer.Get(), 0);

		Graphics::context->PSSetConstantBuffers(2, 1, lightBuffer.GetAddressOf());
	}

	void render() override
	{
		depthStencilView->clear(D3D11_CLEAR_DEPTH);

		Graphics::clearDefRTV(DirectX::Colors::DimGray);

		Graphics::setDefRTV();

		Shader::displayVShader->use();
		tonemapShader->use();

		Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());
		envTexture->setSRV(0);

		Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Graphics::context->Draw(3, 0);

		Graphics::setDefRTV(depthStencilView->get());

		Graphics::setBlendState(StateCommon::defBlendState.Get());
		modelVShader->use();
		modelPShader->use();

		Graphics::context->IASetInputLayout(inputLayout.Get());

		for (unsigned i = 0; i < numModels; i++)
		{
			models[i].draw();
		}

	}
};