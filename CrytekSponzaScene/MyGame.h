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

	RenderTexture* ssaoTexture;

	RenderTexture* ssaoBlured;

	RenderTexture* albedo;

	Texture2D* randomNormal;

	DepthStencilView* depthStencilView;

	ComPtr<ID3D11InputLayout> inputLayout;

	Shader* ssaoShader;

	Shader* ssaoBlur;

	Shader* deferredFinal;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Scene* scene;

	ComPtr<ID3D11SamplerState> wrapSampler;

	ComPtr<ID3D11Buffer> lightBuffer;

	DirectX::XMFLOAT4 eye = { 0.0f,20.f,0.f,1.f };

	float theta = 0.f;

	float theta2 = 0.f;

	static constexpr float moveSpeed = 40.f;

	static constexpr DirectX::XMVECTORF32 up = { 0.f, 1.f, 0.f , 0.f };

	struct LightInfo
	{
		DirectX::XMFLOAT4 viewPos;
		struct Light
		{
			DirectX::XMFLOAT4 position;
			DirectX::XMFLOAT4 color;
			float radius;
			float quadraticFalloff;
			float linearFalloff;
			float v0;
		}lights[17]{};
	}lightInfo{};

	MyGame() :
		deferredVShader(Shader::fromFile("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(Shader::fromFile("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(Shader::fromFile("DeferredFinal.hlsl", ShaderType::Pixel)),
		ssaoBlur(Shader::fromFile("SSAOBlur.hlsl", ShaderType::Pixel)),
		positionDepth(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false)),
		normalSpecular(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false)),
		albedo(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false)),
		ssaoTexture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_FLOAT, DirectX::Colors::Black, false)),
		ssaoBlured(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_FLOAT, DirectX::Colors::Black, false)),
		depthStencilView(DepthStencilView::create(DXGI_FORMAT_D32_FLOAT, false)),
		randomNormal(Texture2D::create("RandomNormal.png")),
		ssaoShader(Shader::fromFile("SSAOShader.hlsl", ShaderType::Pixel)),
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

		{
			auto setLight = [](LightInfo::Light* light, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& color, const float& radius)
			{
				light->position = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1.f);
				light->color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.f);
				light->radius = radius;
			};

			DirectX::XMFLOAT3 lightColors[5];
			lightColors[0] = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
			lightColors[1] = DirectX::XMFLOAT3(1.0f, 0.7f, 0.7f);
			lightColors[2] = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
			lightColors[3] = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
			lightColors[4] = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

			for (int i = 0; i < 5; i++)
			{
				setLight(&lightInfo.lights[i], { (float)(i - 2.5f) * 50.0f, 20.0f, 0.0f }, lightColors[i], 120.0f);
			}

			setLight(&lightInfo.lights[5], { -48.75f, 16.0f, -17.8f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[6], { -48.75f, 16.0f,  18.4f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[7], { 62.0f, 16.0f, -17.8f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[8], { 62.0f, 16.0f,  18.4f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[9], { 120.0f, 20.0f, -43.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[10], { 120.0f, 20.0f, 41.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[11], { -110.0f, 20.0f, -43.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[12], { -110.0f, 20.0f, 41.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[13], { -122.0f, 18.0f, -3.2f }, { 1.0f, 0.3f, 0.3f }, 25.0f);
			setLight(&lightInfo.lights[14], { -122.0f, 18.0f,  3.2f }, { 0.3f, 1.0f, 0.3f }, 25.0f);
			setLight(&lightInfo.lights[15], { 135.0f, 18.0f, -3.2f }, { 0.3f, 0.3f, 1.0f }, 25.0f);
			setLight(&lightInfo.lights[16], { 135.0f, 18.0f,  3.2f }, { 1.0f, 1.0f, 0.3f }, 25.0f);
		}

		{
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(LightInfo);
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			Graphics::device->CreateBuffer(&bd, nullptr, lightBuffer.ReleaseAndGetAddressOf());
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

		Camera::setProj(DirectX::XMMatrixPerspectiveFovLH(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f));
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
		delete randomNormal;
		delete ssaoShader;
		delete ssaoTexture;
		delete ssaoBlured;
		delete ssaoBlur;
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
		else if (Keyboard::getKeyDown(Keyboard::A))
		{
			const DirectX::XMFLOAT2 leftVector = DirectX::XMFLOAT2(cosf(theta + Math::half_pi), sinf(theta + Math::half_pi));
			eye.x += leftVector.x * dt * moveSpeed;
			eye.z += leftVector.y * dt * moveSpeed;
		}
		else if (Keyboard::getKeyDown(Keyboard::D))
		{
			const DirectX::XMFLOAT2 rightVector = DirectX::XMFLOAT2(cosf(theta - Math::half_pi), sinf(theta - Math::half_pi));
			eye.x += rightVector.x * dt * moveSpeed;
			eye.z += rightVector.y * dt * moveSpeed;
		}

		lightInfo.viewPos = DirectX::XMFLOAT4(eye.x, eye.y, eye.z, 1.f);
		lightInfo.lights[0].position = DirectX::XMFLOAT4(-sinf(360.0f * Graphics::getSTime()/2.f * Math::degToRad) * 120.0f, 20.5f, cosf(360.0f * Graphics::getSTime() * 8.0f * Math::degToRad/2.f) * 10.0f, 1.f);
		lightInfo.lights[0].color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		
		D3D11_MAPPED_SUBRESOURCE mappedData;
		Graphics::context->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &lightInfo, sizeof(LightInfo));
		Graphics::context->Unmap(lightBuffer.Get(), 0);


		DirectX::XMFLOAT4 focus = DirectX::XMFLOAT4(eye.x + focusVector.x, eye.y + focusVector.y, eye.z + focusVector.z, 1.f);

		Camera::setView(DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&eye), DirectX::XMLoadFloat4(&focus), up));

		Graphics::context->PSSetConstantBuffers(3, 1, lightBuffer.GetAddressOf());
	}

	void render()
	{
		positionDepth->clearRTV(DirectX::Colors::Black);
		normalSpecular->clearRTV(DirectX::Colors::Black);
		albedo->clearRTV(DirectX::Colors::Black);

		Graphics::setBlendState(StateCommon::blendReplace.Get());

		depthStencilView->clear(D3D11_CLEAR_DEPTH);
		RenderTexture::setRTVs({ positionDepth,normalSpecular,albedo }, depthStencilView->get());

		Graphics::context->IASetInputLayout(inputLayout.Get());

		deferredVShader->use();
		deferredPShader->use();

		Graphics::context->PSSetSamplers(0, 1, wrapSampler.GetAddressOf());

		scene->draw();

		Graphics::context->PSSetSamplers(1, 1, StateCommon::defSamplerState.GetAddressOf());

		ssaoTexture->clearRTV(DirectX::Colors::Black);
		ssaoTexture->setRTV();

		positionDepth->getTexture()->setSRV(0);
		normalSpecular->getTexture()->setSRV(1);
		randomNormal->setSRV(2);

		Graphics::setBlendState(StateCommon::defBlendState.Get());

		Shader::displayVShader->use();
		ssaoShader->use();

		Graphics::context->Draw(3, 0);

		ssaoBlured->clearRTV(DirectX::Colors::Black);
		ssaoBlured->setRTV();

		ssaoTexture->getTexture()->setSRV(0);

		Shader::displayVShader->use();
		ssaoBlur->use();

		Graphics::context->Draw(3, 0);

		Graphics::clearDefRTV(DirectX::Colors::Black);
		Graphics::setDefRTV();

		positionDepth->getTexture()->setSRV(0);
		normalSpecular->getTexture()->setSRV(1);
		albedo->getTexture()->setSRV(2);
		ssaoBlured->getTexture()->setSRV(3);

		Shader::displayVShader->use();
		deferredFinal->use();

		Graphics::context->Draw(3, 0);

		ID3D11ShaderResourceView* nullView[4] = { nullptr,nullptr,nullptr,nullptr };

		Graphics::context->PSSetShaderResources(0, 4, nullView);
	}


};