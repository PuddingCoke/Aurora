#pragma once

#include<Aurora/Game.h>
#include<Aurora/Resource/SwapTexture.h>
#include<Aurora/Utils/Random.h>
#include<Aurora/Utils/Math.h>
#include<Aurora/Effect/FadeEffect.h>
#include<Aurora/Effect/BloomEffect.h>

#include<Aurora/Resource/ComputeVertexBuffer.h>
#include<Aurora/Resource/ResourceBuffer.h>

class MyGame :public Game
{
public:

	static constexpr UINT vehicleNum = 1000;

	ComputeVertexBuffer* positionVelocity;

	ResourceBuffer* maxSpeedMaxForce;

	RenderTexture* renderTexture;

	ResourceTexture* resolvedTexture;

	ResourceTexture* arrowTexture;

	BloomEffect bloomEffect;

	struct SimulationParam
	{
		UINT vehicleNum;
		FLOAT speedMultiply;
		DirectX::XMFLOAT2 padding;
	} param;

	ConstantBuffer* simulationParamBuffer;

	Shader* stepCS;

	Shader* vehicleVS;

	Shader* vehicleGS;

	Shader* vehiclePS;

	ComPtr<ID3D11InputLayout> inputLayout;

	MyGame() :
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN, DirectX::Colors::Black, true)),
		resolvedTexture(new ResourceTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN, D3D11_USAGE_DEFAULT)),
		arrowTexture(new ResourceTexture("arrow.png")),
		bloomEffect(ImCtx::get(), Graphics::getWidth(), Graphics::getHeight()),
		stepCS(new Shader("StepCS.hlsl", ShaderType::Compute)),
		vehicleVS(new Shader("VehicleVS.hlsl", ShaderType::Vertex)),
		vehicleGS(new Shader("VehicleGS.hlsl", ShaderType::Geometry)),
		vehiclePS(new Shader("VehiclePS.hlsl", ShaderType::Pixel)),
		param{}
	{
		bloomEffect.setExposure(1.3f);
		bloomEffect.setGamma(1.4f);
		bloomEffect.setIntensity(1.f);
		bloomEffect.setThreshold(0.0f);
		bloomEffect.applyChange();

		DirectX::XMFLOAT4* positionVelocityArray = new DirectX::XMFLOAT4[vehicleNum];
		DirectX::XMFLOAT4* maxSpeedMaxForceArray = new DirectX::XMFLOAT4[vehicleNum];

		for (size_t i = 0; i < vehicleNum; i++)
		{
			float angle = Random::Float() * Math::two_pi;
			float xSpeed = 3.f * cosf(angle);
			float ySpeed = 3.f * sinf(angle);
			positionVelocityArray[i] = DirectX::XMFLOAT4(Random::Float() * Graphics::getWidth(), Random::Float() * Graphics::getHeight(), xSpeed, ySpeed);
			maxSpeedMaxForceArray[i] = DirectX::XMFLOAT4(3.f, 0.1f, 0.f, 0.f);
		}

		positionVelocity = new ComputeVertexBuffer(sizeof(DirectX::XMFLOAT4) * vehicleNum, FMT::RGBA32F, positionVelocityArray);
		maxSpeedMaxForce = new ResourceBuffer(sizeof(DirectX::XMFLOAT4) * vehicleNum, D3D11_USAGE_DEFAULT, FMT::RGBA32F, maxSpeedMaxForceArray);

		delete[] positionVelocityArray;
		delete[] maxSpeedMaxForceArray;

		param.vehicleNum = vehicleNum;
		param.speedMultiply = 120.f;

		simulationParamBuffer = new ConstantBuffer(sizeof(SimulationParam), D3D11_USAGE_IMMUTABLE, &param);

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"VELOCITY",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		GraphicsDevice::get()->createInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(vehicleVS), inputLayout.ReleaseAndGetAddressOf());
	}

	~MyGame()
	{
		delete positionVelocity;
		delete maxSpeedMaxForce;
		delete simulationParamBuffer;
		delete renderTexture;
		delete resolvedTexture;
		delete arrowTexture;
		delete stepCS;
		delete vehicleVS;
		delete vehicleGS;
		delete vehiclePS;
	}

	void imGUICall() override
	{
		bloomEffect.imGUIEffectModifier();
	}

	void update(const float& dt) override
	{
		ImCtx::get()->CSSetUAV({ positionVelocity }, 0);
		ImCtx::get()->CSSetSRV({ maxSpeedMaxForce }, 0);

		ImCtx::get()->CSSetConstantBuffer({ simulationParamBuffer }, 1);

		ImCtx::get()->BindShader(stepCS);

		ImCtx::get()->Dispatch(((vehicleNum + 999) & ~999) / 1000, 1, 1);
	}

	void render()
	{
		ImCtx::get()->OMSetBlendState(States::addtiveBlend);

		ImCtx::get()->ClearRTV(renderTexture->getMip(0), DirectX::Colors::Black);
		ImCtx::get()->OMSetRTV({ renderTexture->getMip(0) }, nullptr);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		ImCtx::get()->IASetInputLayout(inputLayout.Get());

		ImCtx::get()->IASetVertexBuffer({ positionVelocity }, { sizeof(DirectX::XMFLOAT4) }, { 0 });

		ImCtx::get()->PSSetSRV({ arrowTexture }, 0);

		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(vehicleVS);
		ImCtx::get()->BindShader(vehicleGS);
		ImCtx::get()->BindShader(vehiclePS);

		ImCtx::get()->Draw(vehicleNum, 0);

		ImCtx::get()->GSUnbindShader();

		ImCtx::get()->ResolveSubresource(resolvedTexture, 0, renderTexture, 0, renderTexture->getFormat());

		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ShaderResourceView* bloomTextureSRV = bloomEffect.process(resolvedTexture);

		ImCtx::get()->OMSetBlendState(nullptr);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);
		ImCtx::get()->PSSetSRV({ bloomTextureSRV }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(Shader::fullScreenPS);

		ImCtx::get()->DrawQuad();
	}


};