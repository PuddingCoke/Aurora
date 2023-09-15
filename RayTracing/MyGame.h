#pragma once

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/ImCtx.h>
#include<Aurora/Utils/Math.h>
#include<Aurora/Resource/RenderTexture.h>
#include<Aurora/Resource/ComputeTexture.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Shader* rayTracingPS;

	Shader* displayPS;

	ConstantBuffer* cameraParamBuffer;

	float targetRadius;

	RenderTexture* renderTexture;

	ComputeTexture* randomTexture;

	UINT frameIndex;

	struct CameraParam
	{
		float phi;
		float theta;
		float radius;
		float POWER;
	} cameraParam;

	MyGame() :
		rayTracingPS(new Shader(Utils::getRootFolder() + "RayTracingPS.cso", ShaderType::Pixel)),
		displayPS(new Shader("DisplayPS.hlsl", ShaderType::Pixel)),
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16UN)),
		randomTexture(new ComputeTexture(1, 1, FMT::RG32F, FMT::RG32F, FMT::RG32F, 1, 1)),
		cameraParam{ 0.25f,0.0f,12.0f,0.1f },
		frameIndex(0)
	{
		targetRadius = cameraParam.radius;

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					cameraParam.phi -= Mouse::getDY() * Graphics::getDeltaTime();
					cameraParam.theta += Mouse::getDX() * Graphics::getDeltaTime();
					cameraParam.phi = Math::clamp(cameraParam.phi, 0.01f, Math::half_pi - 0.01f);

					frameIndex = 0;
				}
			});

		Mouse::addScrollEvent([this]()
			{
				targetRadius -= Mouse::getWheelDelta() * 1.f;

				frameIndex = 0;
			});

		//prev*(1.0-1.0/frameCount)+cur*(1.0/frameCount)

		Graphics::setRecordConfig(1800, 60);

		cameraParamBuffer = new ConstantBuffer(sizeof(CameraParam), D3D11_USAGE_DYNAMIC, &cameraParam);
	}

	~MyGame()
	{
		delete rayTracingPS;
		delete displayPS;
		delete cameraParamBuffer;
		delete renderTexture;
		delete randomTexture;
	}

	void imGUICall() override
	{
		ImGui::Text("Frame rendered %d", frameIndex);
	}

	void update(const float& dt) override
	{
		cameraParam.radius = targetRadius;
		//cameraParam.theta += dt * 0.5f;

		memcpy(ImCtx::get()->Map(cameraParamBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &cameraParam, sizeof(CameraParam));
		ImCtx::get()->Unmap(cameraParamBuffer, 0);
	}

	void render()
	{
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->OMSetBlendState(States::defBlendState);

		ImCtx::get()->PSSetConstantBuffer({ cameraParamBuffer }, 1);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);

		//ImCtx::get()->ClearRTV(renderTexture->getMip(0), DirectX::Colors::Black);

		ImCtx::get()->OMSetRTV({ renderTexture->getMip(0) }, nullptr);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		ImCtx::get()->PSSetSRV({ randomTexture }, 0);

		ImCtx::get()->BindShader(rayTracingPS);

		frameIndex++;

		const float clearValue[4] = { frameIndex,Random::Float() * 1000.f,0.f,0.f };

		ImCtx::get()->ClearUAV(randomTexture->getMip(0), clearValue);

		ImCtx::get()->DrawQuad();

		/*for (UINT i = 0; i < 480; i++)
		{
			const float clearValue[4] = { i + 1,Random::Float() * 100.f,0.f,0.f };

			ImCtx::get()->ClearUAV(randomTexture->getMip(0), clearValue);

			ImCtx::get()->DrawQuad();
		}*/

		ImCtx::get()->OMSetBlendState(nullptr);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);

		ImCtx::get()->PSSetSRV({ renderTexture }, 0);

		ImCtx::get()->BindShader(displayPS);

		ImCtx::get()->DrawQuad();
	}


};