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

	static constexpr DirectX::XMVECTORF32 up = { 0.f, 0.f, 1.f , 0.f };

	static constexpr DirectX::XMVECTORF32 focusPoint = { 0.f,0.f,0.5f,1.f };

	static constexpr DirectX::XMVECTORF32 eye = { 0.f,-1.f,1.f,1.f };

	float theta = 0.1f;

	struct LightInfo
	{
		DirectX::XMFLOAT3 lightPos;
		float v0;
		DirectX::XMFLOAT3 viewPos;
		float v1;
	} lightInfo{};

	ComPtr<ID3D11Buffer> lightBuffer;

	MyGame() :
		models(Model::create("untitled.obj", numModels)),
		modelVShader(Shader::fromFile("ModelVShader.hlsl", ShaderType::Vertex)),
		modelPShader(Shader::fromFile("ModelPShader.hlsl", ShaderType::Pixel)),
		depthStencilView(DepthStencilView::create(DXGI_FORMAT_D32_FLOAT))
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

	}

	~MyGame()
	{
		delete[] models;
		delete modelVShader;
		delete modelPShader;
		delete depthStencilView;
	}

	void update(const float& dt) override
	{
		theta += dt;

		const DirectX::XMVECTOR rotPoint = DirectX::XMVector4Transform(eye, DirectX::XMMatrixRotationAxis(up, theta));

		Graphics::setView(DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(rotPoint, focusPoint, up)));

		DirectX::XMFLOAT4 eyeRotated;
		DirectX::XMStoreFloat4(&eyeRotated, rotPoint);

		lightInfo.lightPos = DirectX::XMFLOAT3(eyeRotated.x, eyeRotated.y, eyeRotated.z);
		lightInfo.viewPos = DirectX::XMFLOAT3(eyeRotated.x, eyeRotated.y, eyeRotated.z);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		Graphics::context->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &lightInfo, sizeof(LightInfo));
		Graphics::context->Unmap(lightBuffer.Get(), 0);

		Graphics::context->PSSetConstantBuffers(2, 1, lightBuffer.GetAddressOf());
	}

	void render() override
	{
		Graphics::context->ClearDepthStencilView(depthStencilView->get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		Graphics::setDefRTV(depthStencilView->get());
		Graphics::clearDefRTV(DirectX::Colors::Black);

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