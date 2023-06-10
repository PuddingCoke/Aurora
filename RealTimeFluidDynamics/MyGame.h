#pragma once

#include<Aurora/Game.h>
#include<Aurora/Utils/Color.h>
#include<Aurora/Utils/Timer.h>

#include<Aurora/EngineAPI/ResourceEssential.h>

#include<Aurora/Effect/BloomEffect.h>

#include<Aurora/Resource/SwapTexture.h>

//基本思想 https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-38-fast-fluid-dynamics-simulation-gpu
//有些公式看不懂可以看这个 http://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf
//涡流的具体实现在这里 https://softologyblog.wordpress.com/2019/03/13/vorticity-confinement-for-eulerian-fluid-simulations/

class MyGame :public Game
{
public:

	struct Config
	{
		float colorChangeSpeed = 10.f;//颜色改变速度
		float colorDissipationSpeed = 1.f;//颜色消散速度
		float velocityDissipationSpeed = 0.2f;//速度消散速度
		float pressureDissipationSpeed = 0.25f;//压力消散速度
		float curlIntensity = 80.f;//涡流强度
		float splatRadius = 0.25f;//施加颜色的半径
		float splatForce = 6000.f;//施加速度的强度
		const unsigned int pressureIteraion = 50;//雅可比迭代次数 这个值越高物理模拟越不容易出错 NVIDIA的文章有提到通常20-50次就够了
		const unsigned int colorRes = Graphics::getHeight();//颜色模拟分辨率
		const unsigned int simRes = Graphics::getHeight() / 2;//物理模拟分辨率
	}config;

	struct SimulationDelta
	{
		DirectX::XMFLOAT2 pos = { 0,0 };
		DirectX::XMFLOAT2 posDelta = { 0,0 };
		DirectX::XMFLOAT4 splatColor = { 0,0,0,1 };
	}simulationDelta;//变化的值如 位置 颜色

	struct SimulationParam
	{
		DirectX::XMFLOAT2 screenTexelSize;
		DirectX::XMFLOAT2 simTexelSize;
		float colorDissipationSpeed;
		float velocityDissipationSpeed;
		float pressureDissipationSpeed;
		float curlIntensity;
		float aspectRatio;
		float splatRadius;
		DirectX::XMFLOAT2 v0;
	}simulationParam;//模拟属性

	SwapTexture<RenderTexture>* colorTex;//颜色

	SwapTexture<RenderTexture>* velocityTex;//速度

	RenderTexture* curlTex;//旋度

	RenderTexture* divergenceTex;//散度

	SwapTexture<RenderTexture>* pressureTex;//压力

	ConstantBuffer* simulationParamBuffer;

	ConstantBuffer* simulationDeltaBuffer;

	Timer colorUpdateTimer;

	ComPtr<ID3D11BlendState> blendState;

	RenderTexture* originTexture;

	BloomEffect bloomEffect;

	Shader* fluidFinalPS;
	Shader* splatColorPS;
	Shader* splatVelocityPS;
	Shader* colorAdvectionDissipationPS;
	Shader* velocityAdvectionDissipationPS;
	Shader* pressureDissipationPS;
	Shader* curlPS;
	Shader* divergencePS;
	Shader* pressureGradientSubtractPS;
	Shader* viscousDiffusionPS;
	Shader* vorticityPS;

	bool colorChanging;

	MyGame() :
		colorUpdateTimer(1.f),
		fluidFinalPS(new Shader("FluidFinalPS.hlsl", ShaderType::Pixel)),
		splatColorPS(new Shader("SplatColorPS.hlsl", ShaderType::Pixel)),
		splatVelocityPS(new Shader("SplatVelocityPS.hlsl", ShaderType::Pixel)),
		colorAdvectionDissipationPS(new Shader("ColorAdvectionDissipation.hlsl", ShaderType::Pixel)),
		velocityAdvectionDissipationPS(new Shader("VelocityAdvectionDissipation.hlsl", ShaderType::Pixel)),
		pressureDissipationPS(new Shader("PressureDissipationPS.hlsl", ShaderType::Pixel)),
		curlPS(new Shader("CurlPS.hlsl", ShaderType::Pixel)),
		divergencePS(new Shader("DivergencePS.hlsl", ShaderType::Pixel)),
		pressureGradientSubtractPS(new Shader("PressureGradientSubtractPS.hlsl", ShaderType::Pixel)),
		viscousDiffusionPS(new Shader("ViscousDiffusionPS.hlsl", ShaderType::Pixel)),
		vorticityPS(new Shader("VorticityPS.hlsl", ShaderType::Pixel)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		colorChanging(true)
	{
		bloomEffect.setThreshold(0.f);
		bloomEffect.applyChange();

		{
			D3D11_BLEND_DESC blendStateDesc = {};
			blendStateDesc.IndependentBlendEnable = false;
			blendStateDesc.RenderTarget[0].BlendEnable = true;
			blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

			Renderer::getDevice()->CreateBlendState(&blendStateDesc, blendState.ReleaseAndGetAddressOf());
		}

		{
			const DirectX::XMUINT2 colorRes = { (UINT)(config.colorRes * Graphics::getAspectRatio()), config.colorRes };
			const DirectX::XMUINT2 simRes = { (UINT)(config.simRes * Graphics::getAspectRatio()), config.simRes };


			colorTex = new SwapTexture<RenderTexture>([=] {return new RenderTexture(colorRes.x, colorRes.y, FMT::RGBA16F); });

			velocityTex = new SwapTexture<RenderTexture>([=] {return new RenderTexture(simRes.x, simRes.y, FMT::RG16F); });

			pressureTex = new SwapTexture<RenderTexture>([=] {return new RenderTexture(simRes.x, simRes.y, FMT::R16F); });

			curlTex = new RenderTexture(simRes.x, simRes.y, FMT::R16F);
			divergenceTex = new RenderTexture(simRes.x, simRes.y, FMT::R16F);

			simulationParam =
			{
				{1.f / Graphics::getWidth(),1.f / Graphics::getHeight()},
				{1.f / simRes.x,1.f / simRes.y},
				config.colorDissipationSpeed,
				config.velocityDissipationSpeed,
				config.pressureDissipationSpeed,
				config.curlIntensity,
				Graphics::getAspectRatio(),
				config.splatRadius / 100.f * Graphics::getAspectRatio(),
				{0,0}
			};

			simulationParamBuffer = new ConstantBuffer(sizeof(SimulationParam), D3D11_USAGE_DYNAMIC, &simulationParam);
			simulationDeltaBuffer = new ConstantBuffer(sizeof(SimulationDelta), D3D11_USAGE_DYNAMIC, &simulationDelta);
		}

		Mouse::addLeftDownEvent([this]()
			{
				simulationDelta.pos = { (float)Mouse::getX() / Graphics::getWidth(), (float)(Graphics::getHeight() - Mouse::getY()) / Graphics::getHeight() };
				if (colorChanging)
				{
					Color c = Color::HSVtoRGB({ Random::Float(),1.f,1.f });
					simulationDelta.splatColor = { c.r,c.g,c.b,1.f };
				}
			});

		Mouse::addMoveEvent([this]() {
			if (Mouse::getLeftDown())
			{
				const DirectX::XMFLOAT2 pos =
				{
					(float)Mouse::getX() / Graphics::getWidth(),
					(float)(Graphics::getHeight() - Mouse::getY()) / Graphics::getHeight()
				};

				const DirectX::XMFLOAT2 posDelta =
				{
					(pos.x - simulationDelta.pos.x) * config.splatForce,
					((pos.y - simulationDelta.pos.y) / Graphics::getAspectRatio()) * config.splatForce
				};

				simulationDelta.pos = pos;
				simulationDelta.posDelta = posDelta;
				splat();
			}
			});
	}

	~MyGame()
	{
		delete colorTex;
		delete velocityTex;
		delete curlTex;
		delete divergenceTex;
		delete pressureTex;
		delete simulationParamBuffer;
		delete simulationDeltaBuffer;
		delete fluidFinalPS;
		delete splatColorPS;
		delete splatVelocityPS;
		delete colorAdvectionDissipationPS;
		delete velocityAdvectionDissipationPS;
		delete pressureDissipationPS;
		delete curlPS;
		delete divergencePS;
		delete pressureGradientSubtractPS;
		delete viscousDiffusionPS;
		delete vorticityPS;
		delete originTexture;
	}

	void splat()
	{
		memcpy(simulationDeltaBuffer->map().pData, &simulationDelta, sizeof(SimulationDelta));
		simulationDeltaBuffer->unmap();

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->PSSetConstantBuffer({ simulationParamBuffer,simulationDeltaBuffer }, 1);
		RenderAPI::get()->PSSetSampler({ States::pointClampSampler,States::linearClampSampler }, 0);
		RenderAPI::get()->OMSetBlendState(blendState.Get());
		RenderAPI::fullScreenVS->use();

		//施加颜色和速度

		RenderAPI::get()->RSSetViewport(velocityTex->width, velocityTex->height);
		RenderAPI::get()->OMSetRTV({ velocityTex->write()->getRTVMip(0) }, nullptr);
		splatVelocityPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		velocityTex->swap();

		RenderAPI::get()->RSSetViewport(colorTex->width, colorTex->height);
		RenderAPI::get()->OMSetRTV({ colorTex->write()->getRTVMip(0) }, nullptr);
		splatColorPS->use();
		RenderAPI::get()->PSSetSRV({ colorTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		colorTex->swap();
	}

	void step()
	{
		RenderAPI::get()->RSSetViewport(velocityTex->width, velocityTex->height);
		RenderAPI::get()->PSSetConstantBuffer({ simulationParamBuffer,simulationDeltaBuffer }, 1);
		RenderAPI::get()->PSSetSampler({ States::pointClampSampler,States::linearClampSampler }, 0);
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::fullScreenVS->use();

		//求旋度
		RenderAPI::get()->OMSetRTV({ curlTex->getRTVMip(0) }, nullptr);
		curlPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();

		//通过已经求得的散度对速度图施加漩涡
		RenderAPI::get()->OMSetRTV({ velocityTex->write()->getRTVMip(0) }, nullptr);
		vorticityPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read(),curlTex }, 0);
		RenderAPI::get()->DrawQuad();
		velocityTex->swap();

		//求散度
		RenderAPI::get()->OMSetRTV({ divergenceTex->getRTVMip(0) }, nullptr);
		divergencePS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();

		//对压力图进行消散处理
		RenderAPI::get()->OMSetRTV({ pressureTex->write()->getRTVMip(0) }, nullptr);
		pressureDissipationPS->use();
		RenderAPI::get()->PSSetSRV({ pressureTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		pressureTex->swap();

		//进行雅可比迭代
		viscousDiffusionPS->use();
		for (unsigned int i = 0; i < config.pressureIteraion; i++)
		{
			RenderAPI::get()->OMSetRTV({ pressureTex->write()->getRTVMip(0) }, nullptr);
			RenderAPI::get()->PSSetSRV({ pressureTex->read(),divergenceTex }, 0);
			RenderAPI::get()->DrawQuad();
			pressureTex->swap();
		}

		//速度图减去它的散度来求得没有散度的速度图
		RenderAPI::get()->OMSetRTV({ velocityTex->write()->getRTVMip(0) }, nullptr);
		pressureGradientSubtractPS->use();
		RenderAPI::get()->PSSetSRV({ pressureTex->read(),velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		velocityTex->swap();

		//速度移动并进行消散处理
		RenderAPI::get()->OMSetRTV({ velocityTex->write()->getRTVMip(0) }, nullptr);
		velocityAdvectionDissipationPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		velocityTex->swap();

		//颜色移动并进行消散处理
		RenderAPI::get()->RSSetViewport(colorTex->width, colorTex->height);
		RenderAPI::get()->OMSetRTV({ colorTex->write()->getRTVMip(0) }, nullptr);
		colorAdvectionDissipationPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read(),colorTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		colorTex->swap();
	}

	void imGUICall() override
	{
		ImGui::SliderFloat("Color change speed", &config.colorChangeSpeed, 0.f, 50.f);
		ImGui::SliderFloat("Color dissipation speed", &simulationParam.colorDissipationSpeed, 0.f, 10.f);
		ImGui::SliderFloat("Velocity dissipation speed", &simulationParam.velocityDissipationSpeed, 0.f, 1.f);
		ImGui::SliderFloat("Pressure dissipation speed", &simulationParam.pressureDissipationSpeed, 0.f, 1.f);
		ImGui::SliderFloat("Curl intensity", &simulationParam.curlIntensity, 0.f, 100.f);
		ImGui::SliderFloat("Splat radius", &simulationParam.splatRadius, 0.f, 0.1f);
		ImGui::SliderFloat("Splat force", &config.splatForce, 1000.f, 10000.f);
		ImGui::Checkbox("Color dynamic change", &colorChanging);
		ImGui::ColorPicker4("Splat color", (float*)&(simulationDelta.splatColor.x), ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_DisplayHSV);
		//bloomEffect.imGUIEffectModifier();
	}

	void update(const float& dt) override
	{
	}

	void render()
	{
		memcpy(simulationParamBuffer->map().pData, &simulationParam, sizeof(SimulationParam));
		simulationParamBuffer->unmap();

		//物理模拟

		if (colorUpdateTimer.update(Graphics::getDeltaTime() * config.colorChangeSpeed))
		{
			if (colorChanging)
			{
				Color c = Color::HSVtoRGB({ Random::Float(),1.f,1.f });
				simulationDelta.splatColor = { c.r,c.g,c.b,1.f };
			}
		}

		step();

		//最后进行简单的渲染
		RenderAPI::get()->OMSetBlendState(blendState.Get());
		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		originTexture->clearRTV(DirectX::Colors::Black, 0);
		RenderAPI::get()->OMSetRTV({ originTexture->getRTVMip(0) }, nullptr);
		fluidFinalPS->use();
		RenderAPI::get()->PSSetSRV({ colorTex->read() }, 0);
		RenderAPI::get()->DrawQuad();

		ShaderResourceView* const bloomSRV = bloomEffect.process(originTexture);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->PSSetSampler({ States::pointClampSampler,States::linearClampSampler }, 0);
		RenderAPI::get()->PSSetConstantBuffer({ simulationParamBuffer,simulationDeltaBuffer }, 1);
		RenderAPI::get()->PSSetSRV({ bloomSRV }, 0);
		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();
	}


};