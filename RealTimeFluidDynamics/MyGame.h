#pragma once

#include<Aurora/Game.h>
#include<Aurora/DoubleRTV.h>
#include<Aurora/Color.h>
#include<Aurora/Timer.h>
#include<Aurora/PostProcessing/BloomEffect.h>

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
		const unsigned int simRes = Graphics::getHeight() / 3;//物理模拟分辨率
	}config;

	struct SplatPoint
	{
		float texCoordX = 0;
		float texCoordY = 0;
		float prevTexCoordX = 0;
		float prevTexCoordY = 0;
		float deltaX = 0;
		float deltaY = 0;
		bool moved = false;
		float r = 30.f;
		float g = 0.f;
		float b = 300.f;

		void makeColorRandom()
		{
			const Color c = Color::HSVtoRGB({ Random::Float(),1.f,1.f });
			r = c.r * 0.15f;
			g = c.g * 0.15f;
			b = c.b * 0.15f;
		}

	}splatPoint;

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

	DoubleRTV* colorTex;//颜色

	DoubleRTV* velocityTex;//速度

	RenderTexture* curlTex;//旋度

	RenderTexture* divergenceTex;//散度

	DoubleRTV* pressureTex;//压力

	Buffer* simulationParamBuffer;

	Buffer* simulationDeltaBuffer;

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

	float exposure = 0.6f;
	float gamma = 1.4f;

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
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight())
	{
		bloomEffect.setExposure(exposure);
		bloomEffect.setGamma(gamma);
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

			Renderer::device->CreateBlendState(&blendStateDesc, blendState.ReleaseAndGetAddressOf());
		}

		{
			const DirectX::XMUINT2 colorRes = { (UINT)(config.colorRes * Graphics::getAspectRatio()), config.colorRes };
			const DirectX::XMUINT2 simRes = { (UINT)(config.simRes * Graphics::getAspectRatio()), config.simRes };

			colorTex = new DoubleRTV(colorRes.x, colorRes.y, DXGI_FORMAT_R16G16B16A16_FLOAT);
			velocityTex = new DoubleRTV(simRes.x, simRes.y, DXGI_FORMAT_R16G16_FLOAT);
			pressureTex = new DoubleRTV(simRes.x, simRes.y, DXGI_FORMAT_R16_FLOAT);
			curlTex = new RenderTexture(simRes.x, simRes.y, DXGI_FORMAT_R16_FLOAT);
			divergenceTex = new RenderTexture(simRes.x, simRes.y, DXGI_FORMAT_R16_FLOAT);

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

			simulationParamBuffer = new Buffer(sizeof(SimulationParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, &simulationParam, D3D11_CPU_ACCESS_WRITE);
			simulationDeltaBuffer = new Buffer(sizeof(SimulationDelta), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, &simulationDelta, D3D11_CPU_ACCESS_WRITE);
		}

		Mouse::addLeftDownEvent([this]() {
			updatePointDownData(Mouse::getX(), Graphics::getHeight() - Mouse::getY());
			});

		Mouse::addMoveEvent([this]() {
			if (Mouse::getLeftDown())
			{
				updatePointMoveData(Mouse::getX(), Graphics::getHeight() - Mouse::getY());
			}
			});
	}

	void updatePointDownData(const float& posX, const float& posY)
	{
		splatPoint.moved = false;
		splatPoint.texCoordX = posX / Graphics::getWidth();
		splatPoint.texCoordY = posY / Graphics::getHeight();
		splatPoint.prevTexCoordX = splatPoint.texCoordX;
		splatPoint.prevTexCoordY = splatPoint.texCoordY;
		splatPoint.deltaX = 0;
		splatPoint.deltaY = 0;
		splatPoint.makeColorRandom();
	}

	void updatePointMoveData(const float& posX, const float& posY)
	{
		splatPoint.prevTexCoordX = splatPoint.texCoordX;
		splatPoint.prevTexCoordY = splatPoint.texCoordY;
		splatPoint.texCoordX = posX / Graphics::getWidth();
		splatPoint.texCoordY = posY / Graphics::getHeight();
		splatPoint.deltaX = splatPoint.texCoordX - splatPoint.prevTexCoordX;
		splatPoint.deltaY = (splatPoint.texCoordY - splatPoint.prevTexCoordY) / Graphics::getAspectRatio();
		splatPoint.moved = fabsf(splatPoint.deltaX) > 0 || fabsf(splatPoint.deltaY) > 0;
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

	void updateColor(const float& dt)
	{
		if (colorUpdateTimer.update(dt * config.colorChangeSpeed))
		{
			splatPoint.makeColorRandom();
		}
	}

	void splat(const float& x, const float& y, const float& dx, const float& dy, const float& r, const float& g, const float& b)
	{
		simulationDelta.pos = DirectX::XMFLOAT2(x, y);
		simulationDelta.posDelta = DirectX::XMFLOAT2(dx, dy);
		simulationDelta.splatColor = DirectX::XMFLOAT4(r, g, b, 1.0);

		memcpy(simulationDeltaBuffer->map(0).pData, &simulationDelta, sizeof(SimulationDelta));
		simulationDeltaBuffer->unmap(0);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->PSSetConstantBuffer({ simulationParamBuffer,simulationDeltaBuffer }, 1);
		RenderAPI::get()->PSSetSampler({ States::pointClampSampler,States::linearClampSampler }, 0);
		RenderAPI::get()->OMSetBlendState(blendState.Get());
		RenderAPI::fullScreenVS->use();

		//施加颜色和速度

		RenderAPI::get()->RSSetViewport(velocityTex->width, velocityTex->height);
		RenderAPI::get()->OMSetRTV({ velocityTex->write() }, nullptr);
		splatVelocityPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		velocityTex->swap();

		RenderAPI::get()->RSSetViewport(colorTex->width, colorTex->height);
		RenderAPI::get()->OMSetRTV({ colorTex->write() }, nullptr);
		splatColorPS->use();
		RenderAPI::get()->PSSetSRV({ colorTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		colorTex->swap();
	}

	void applyInput()
	{
		if (splatPoint.moved)
		{
			splatPoint.moved = false;
			const float dx = splatPoint.deltaX * config.splatForce;
			const float dy = splatPoint.deltaY * config.splatForce;
			splat(splatPoint.texCoordX, splatPoint.texCoordY, dx, dy, splatPoint.r, splatPoint.g, splatPoint.b);
		}
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
		RenderAPI::get()->OMSetRTV({ curlTex }, nullptr);
		curlPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();

		//通过已经求得的散度对速度图施加漩涡
		RenderAPI::get()->OMSetRTV({ velocityTex->write() }, nullptr);
		vorticityPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read(),curlTex }, 0);
		RenderAPI::get()->DrawQuad();
		velocityTex->swap();

		//求散度
		RenderAPI::get()->OMSetRTV({ divergenceTex }, nullptr);
		divergencePS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();

		//对压力图进行消散处理
		RenderAPI::get()->OMSetRTV({ pressureTex->write() }, nullptr);
		pressureDissipationPS->use();
		RenderAPI::get()->PSSetSRV({ pressureTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		pressureTex->swap();

		//进行雅可比迭代
		viscousDiffusionPS->use();
		for (unsigned int i = 0; i < config.pressureIteraion; i++)
		{
			RenderAPI::get()->OMSetRTV({ pressureTex->write() }, nullptr);
			RenderAPI::get()->PSSetSRV({ pressureTex->read(),divergenceTex }, 0);
			RenderAPI::get()->DrawQuad();
			pressureTex->swap();
		}

		//速度图减去它的散度来求得没有散度的速度图
		RenderAPI::get()->OMSetRTV({ velocityTex->write() }, nullptr);
		pressureGradientSubtractPS->use();
		RenderAPI::get()->PSSetSRV({ pressureTex->read(),velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		velocityTex->swap();

		//速度移动并进行消散处理
		RenderAPI::get()->OMSetRTV({ velocityTex->write() }, nullptr);
		velocityAdvectionDissipationPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		velocityTex->swap();

		//颜色移动并进行消散处理
		RenderAPI::get()->RSSetViewport(colorTex->width, colorTex->height);
		RenderAPI::get()->OMSetRTV({ colorTex->write() }, nullptr);
		colorAdvectionDissipationPS->use();
		RenderAPI::get()->PSSetSRV({ velocityTex->read(),colorTex->read() }, 0);
		RenderAPI::get()->DrawQuad();
		colorTex->swap();
	}

	void update(const float& dt) override
	{
		if (Keyboard::getKeyDown(Keyboard::A))
		{
			exposure += 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();

			std::cout << "e " << exposure << "\n";
		}
		else if (Keyboard::getKeyDown(Keyboard::S))
		{
			exposure -= 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();
			std::cout << "e " << exposure << "\n";
		}
		else if (Keyboard::getKeyDown(Keyboard::H))
		{
			gamma += 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
			std::cout << "g " << gamma << "\n";
		}
		else if (Keyboard::getKeyDown(Keyboard::J))
		{
			gamma -= 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
			std::cout << "g " << gamma << "\n";
		}

		//物理模拟
		updateColor(dt);
		applyInput();
		step();
	}

	void render()
	{
		//最后进行简单的渲染
		RenderAPI::get()->OMSetBlendState(blendState.Get());
		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		originTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);
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