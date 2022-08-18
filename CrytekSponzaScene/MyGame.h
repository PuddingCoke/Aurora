#pragma once

#include<Aurora/Game.h>
#include<Aurora/Texture2D.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/DepthStencilView.h>

#include"Scene.h"

class MyGame :public Game
{
public:

	RenderTexture* positionDepth;

	RenderTexture* normalSpecular;

	RenderTexture* albedo;

	DepthStencilView* depthStencilView;

	ComPtr<ID3D11InputLayout> inputLayout;

	Shader* deferredFinal;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Scene* scene;

	ComPtr<ID3D11SamplerState> wrapSampler;

	DirectX::XMFLOAT4 eye = { 0.0f,20.f,0.f,1.f };

	float theta = 0.f;

	float theta2 = 0.f;

	static constexpr DirectX::XMVECTORF32 up = { 0.f, 1.f, 0.f , 0.f };

	MyGame() :
		deferredVShader(Shader::fromFile("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(Shader::fromFile("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(Shader::fromFile("DeferredFinal.hlsl",ShaderType::Pixel)),
		positionDepth(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false)),
		normalSpecular(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false)),
		albedo(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false)),
		depthStencilView(DepthStencilView::create(DXGI_FORMAT_D32_FLOAT, false)),
		scene(Scene::create("scene/sponza.dae"))
	{
		{
			D3D11_INPUT_ELEMENT_DESC layout[5] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			Graphics::device->CreateInputLayout(layout, 5u, deferredVShader->shaderBlob->GetBufferPointer(), deferredVShader->shaderBlob->GetBufferSize(), inputLayout.ReleaseAndGetAddressOf());
		}

		{
			D3D11_SAMPLER_DESC sampDesc = {};
			sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			Graphics::device->CreateSamplerState(&sampDesc, wrapSampler.ReleaseAndGetAddressOf());
		}

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					theta -= Mouse::getDX() * 0.01f;
					theta2 += Mouse::getDY() * 0.01f;
					theta2 = Math::clamp(theta2, -Math::half_pi + 0.1f, Math::half_pi - 0.1f);
				}
			});
	}

	~MyGame()
	{
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete positionDepth;
		delete normalSpecular;
		delete albedo;
		delete depthStencilView;
		delete scene;
	}

	void update(const float& dt) override
	{
		DirectX::XMFLOAT4 focusVector = DirectX::XMFLOAT4(cosf(theta2) * cosf(theta), sinf(theta2), cosf(theta2) * sinf(theta), 1.f);

		const float speedFactor = 100.f;

		if (Keyboard::getKeyDown(Keyboard::W))
		{
			eye.x += speedFactor * focusVector.x * dt;
			eye.y += speedFactor * focusVector.y * dt;
			eye.z += speedFactor * focusVector.z * dt;
		}
		else if (Keyboard::getKeyDown(Keyboard::S))
		{
			eye.x -= speedFactor * focusVector.x * dt;
			eye.y -= speedFactor * focusVector.y * dt;
			eye.z -= speedFactor * focusVector.z * dt;
		}

		DirectX::XMFLOAT4 focus = DirectX::XMFLOAT4(eye.x + focusVector.x, eye.y + focusVector.y, eye.z + focusVector.z, 1.f);

		Camera::setView(DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&eye), DirectX::XMLoadFloat4(&focus), up));
	}

	void render()
	{
		positionDepth->clearRTV(DirectX::Colors::Black);
		normalSpecular->clearRTV(DirectX::Colors::Black);
		albedo->clearRTV(DirectX::Colors::Black);

		Graphics::setBlendState(StateCommon::defBlendState.Get());

		depthStencilView->clear(D3D11_CLEAR_DEPTH);
		RenderTexture::setRTVs({ positionDepth,normalSpecular,albedo }, depthStencilView->get());

		Graphics::context->IASetInputLayout(inputLayout.Get());

		deferredVShader->use();
		deferredPShader->use();

		Graphics::context->PSSetSamplers(0, 1, wrapSampler.GetAddressOf());

		scene->draw();

		Graphics::clearDefRTV(DirectX::Colors::Black);
		Graphics::setDefRTV();

		Graphics::setBlendState(StateCommon::addtiveBlend.Get());
		albedo->getTexture()->setSRV(0);

		Shader::displayVShader->use();
		deferredFinal->use();

		Graphics::context->Draw(3, 0);

		ID3D11ShaderResourceView* nullView = nullptr;

		Graphics::context->PSSetShaderResources(0, 1, &nullView);
	}


};