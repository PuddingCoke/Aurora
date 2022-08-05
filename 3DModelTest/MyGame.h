#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/A3D/Model.h>

class MyGame :public Game
{
public:

	Model* models;

	unsigned int numMeshes;

	Shader* modelVShader;

	Shader* modelPShader;

	ComPtr<ID3D11InputLayout> inputLayout;

	static constexpr DirectX::XMVECTORF32 up = { 0.f, 0.f, 1.f , 0.f };

	static constexpr DirectX::XMVECTORF32 focusPoint = { 0.f,0.f,1.f,1.f };

	static constexpr DirectX::XMVECTORF32 eye = { 0.f,-1.f,2.f,1.f };

	float theta = 0.1f;

	Texture2D* texture;

	MyGame() :
		models(Model::create("untitled.obj", numMeshes)),
		modelVShader(Shader::fromFile("ModelVShader.hlsl", ShaderType::Vertex)),
		modelPShader(Shader::fromFile("ModelPShader.hlsl", ShaderType::Pixel)),
		texture(Texture2D::create(L"Noise.png"))
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

		Graphics::setView(DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eye, focusPoint, up)));
	}

	~MyGame()
	{
		delete[] models;
		delete modelVShader;
		delete modelPShader;
		delete texture;
	}

	void update(const float& dt) override
	{
		theta += dt;

		const DirectX::XMVECTOR rotPoint = DirectX::XMVector4Transform(eye, DirectX::XMMatrixRotationAxis(up, theta));

		Graphics::setView(DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(rotPoint, focusPoint, up)));

	}

	void render() override
	{
		Graphics::setDefRTV();
		Graphics::clearDefRTV(DirectX::Colors::Black);

		Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());
		texture->setSRV(0);

		Graphics::setBlendState(StateCommon::defBlendState.Get());
		modelVShader->use();
		modelPShader->use();

		Graphics::context->IASetInputLayout(inputLayout.Get());

		for (unsigned i = 0; i < numMeshes; i++)
		{
			models[i].draw();
		}

	}
};