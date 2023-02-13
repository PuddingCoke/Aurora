#pragma once

#include<Aurora/Game.h>
#include<Aurora/ComputeTexture.h>
#include<Aurora/ResDepthTexture.h>

#include<Aurora/A3D/DepthCube.h>
#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/A3D/RenderCube.h>

#include<Aurora/PostProcessing/HBAOEffect.h>
#include<Aurora/PostProcessing/BloomEffect.h>

#include"Scene.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	ResDepthTexture* globalShadowTexture;

	ResDepthTexture* depthTexture;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	RenderTexture* originTexture;

	Scene* scene;

	Buffer* lightBuffer;

	Buffer* lightProjBuffer;

	Buffer* cubeProjBuffer;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	Shader* skyboxPShader;

	Shader* cubeRenderVS;

	Shader* cubeRenderPS;

	TextureCube* skybox;

	RenderCube* renderCube;

	Shader* irradianceCompute;

	Shader* irradianceEvaluate;

	ComputeTexture* irradianceCoeff;

	ComputeTexture* depthOctahedralMap;

	StructuredBuffer* irradianceSamples;

	DepthCube* depthCube;

	FPSCamera camera;

	HBAOEffect hbaoEffect;

	BloomEffect bloomEffect;

	static constexpr unsigned int shadowMapRes = 4096;

	struct Light
	{
		DirectX::XMVECTOR lightDir;
		DirectX::XMVECTOR lightColor;
	} light{};

	struct CubeProj
	{
		DirectX::XMMATRIX viewProj[6];
		DirectX::XMFLOAT3 probeLocation;
		unsigned int probeIndex;
	} cubeProj;

	float exposure;

	float gamma;

	float sunAngle;

	bool showRadiance = true;

	MyGame() :
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_SNORM, DirectX::Colors::Black)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		depthTexture(new ResDepthTexture(Graphics::getWidth(), Graphics::getHeight())),
		globalShadowTexture(new ResDepthTexture(shadowMapRes, shadowMapRes)),
		scene(Scene::create(assetPath + "/sponza.dae")),
		deferredVShader(new Shader("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(new Shader("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(new Shader("DeferredFinal.hlsl", ShaderType::Pixel)),
		skyboxPShader(new Shader("SkyboxPShader.hlsl", ShaderType::Pixel)),
		cubeRenderVS(new Shader("CubeRenderVS.hlsl", ShaderType::Vertex)),
		cubeRenderPS(new Shader("CubeRenderPS.hlsl", ShaderType::Pixel)),
		irradianceCompute(new Shader("IrradianceCompute.hlsl", ShaderType::Compute)),
		irradianceEvaluate(new Shader("IrradianceEvaluate.hlsl", ShaderType::Pixel)),
		skybox(new TextureCube(assetPath + "/sky/kloppenheim_05_4k.hdr", 2048)),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		camera({ 0.f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f),
		sunAngle(Math::half_pi - 0.02f),
		lightBuffer(new Buffer(sizeof(Light), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		lightProjBuffer(new Buffer(sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		cubeProjBuffer(new Buffer(sizeof(CubeProj), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		renderCube(new RenderCube(128, DXGI_FORMAT_R16G16B16A16_FLOAT)),
		depthCube(new DepthCube(128)),
		irradianceCoeff(new ComputeTexture(9, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, 1024)),
		depthOctahedralMap(new ComputeTexture(16, 16, DXGI_FORMAT_R16G16_FLOAT, 1024))
	{
		exposure = 1.0f;
		gamma = 1.25f;
		bloomEffect.setThreshold(0.35f);
		bloomEffect.setIntensity(0.90f);
		bloomEffect.setExposure(exposure);
		bloomEffect.setGamma(gamma);
		bloomEffect.applyChange();

		{
			D3D11_INPUT_ELEMENT_DESC layout[5] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(deferredVShader), inputLayout.ReleaseAndGetAddressOf());
		}

		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				showRadiance = !showRadiance;
			}
		);

		//预计算采样点
		{
			struct Sample
			{
				DirectX::XMFLOAT4 direction;
				float Ylm[9];
			};

			const unsigned int sampleCount = 400;
			const unsigned int sampleCountSqrt = 20;
			const double oneoverN = 1.0 / (double)sampleCountSqrt;

			unsigned int i = 0;

			std::vector<Sample> samples(sampleCount);

			for (unsigned int a = 0; a < sampleCountSqrt; a++)
			{
				for (unsigned int b = 0; b < sampleCountSqrt; b++)
				{
					const double x = (a + Random::Double()) * oneoverN;
					const double y = (b + Random::Double()) * oneoverN;
					const double theta = 2.0 * acos(sqrt(1.0 - x));
					const double phi = 2.0 * 3.141592653589793238 * y;

					samples[i].direction = DirectX::XMFLOAT4(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta), 1.0);

					for (int l = 0; l < 3; l++)
					{
						for (int m = -l; m <= l; m++)
						{
							int index = l * (l + 1) + m;
							samples[i].Ylm[index] = SH(l, m, theta, phi);
						}
					}
					++i;
				}
			}

			irradianceSamples = new StructuredBuffer(sizeof(Sample) * samples.size(), sizeof(Sample), D3D11_USAGE_IMMUTABLE, samples.data(), 0);
		}

		updateShadow();
	}

	int factorial(int n)
	{
		const int results[13] = { 1,1,2,6,24,120,720,5040,40320,362880,3628800,39916800,479001600 };
		return results[n];
	}

	double K(int l, int m)
	{
		double temp = ((2.0 * l + 1.0) * factorial(l - m)) / (4.0 * 3.141592653589793238 * factorial(l + m));
		return sqrt(temp);
	}

	double P(int l, int m, double x)
	{
		double pmm = 1.0;
		if (m > 0)
		{
			double somx2 = sqrt((1.0 - x) * (1.0 + x));
			double fact = 1.0;
			for (int i = 1; i <= m; i++) {
				pmm *= (-fact) * somx2;
				fact += 2.0;
			}
		}
		if (l == m)
			return pmm;

		double pmmp1 = x * (2.0 * m + 1.0) * pmm;
		if (l == m + 1)
			return pmmp1;

		double pll = 0.0;
		for (int ll = m + 2; ll <= l; ++ll)
		{
			pll = ((2.0 * ll - 1.0) * x * pmmp1 - (ll + m - 1.0) * pmm) / (ll - m);
			pmm = pmmp1;
			pmmp1 = pll;
		}

		return pll;
	}

	double SH(int l, int m, double theta, double phi)
	{
		const double sqrt2 = sqrt(2.0);
		if (m == 0)        return K(l, 0) * P(l, 0, cos(theta));
		else if (m > 0)    return sqrt2 * K(l, m) * cos(m * phi) * P(l, m, cos(theta));
		else                return sqrt2 * K(l, -m) * sin(-m * phi) * P(l, -m, cos(theta));
	}

	~MyGame()
	{
		delete depthTexture;
		delete globalShadowTexture;

		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete originTexture;

		delete scene;
		delete skybox;

		delete lightBuffer;
		delete lightProjBuffer;
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;

		delete cubeRenderVS;
		delete cubeRenderPS;
		delete cubeProjBuffer;

		delete skyboxPShader;

		delete irradianceCompute;
		delete irradianceEvaluate;
		delete irradianceSamples;

		delete renderCube;
		delete depthCube;

		delete irradianceCoeff;
		delete depthOctahedralMap;
	}

	void imGUICall() override
	{
		bloomEffect.imGUIEffectModifier();
	}

	void updateShadow()
	{
		const float xSize = 183;
		const float ySize = 130;
		const float distance = 260.f;
		const DirectX::XMVECTOR offset = { 6.5f,0.f,0.f };

		light.lightDir = { 0.f,sinf(sunAngle),cosf(sunAngle),0.f };
		light.lightColor = DirectX::Colors::White;

		light.lightDir = DirectX::XMVector3Normalize(light.lightDir);

		const DirectX::XMVECTOR lightCamPos = DirectX::XMVectorAdd(DirectX::XMVectorScale(light.lightDir, distance), offset);
		const DirectX::XMMATRIX lightProjMat = DirectX::XMMatrixOrthographicLH(xSize, ySize, 1.f, 512.f);
		const DirectX::XMMATRIX lightViewMat = DirectX::XMMatrixLookAtLH(lightCamPos, offset, { 0.f,1.f,0.f });
		const DirectX::XMMATRIX lightMat = DirectX::XMMatrixTranspose(lightViewMat * lightProjMat);

		memcpy(lightBuffer->map(0).pData, &light, sizeof(Light));
		lightBuffer->unmap(0);

		memcpy(lightProjBuffer->map(0).pData, &lightMat, sizeof(DirectX::XMMATRIX));
		lightProjBuffer->unmap(0);

		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());

		RenderAPI::get()->RSSetState(States::rasterShadow);
		RenderAPI::get()->RSSetViewport(shadowMapRes, shadowMapRes);

		globalShadowTexture->clearDSV(D3D11_CLEAR_DEPTH);

		RenderAPI::get()->OMSetRTV({}, globalShadowTexture);
		RenderAPI::get()->VSSetConstantBuffer({ lightProjBuffer }, 2);

		scene->renderGeometry(RenderAPI::shadowVS);
		RenderAPI::get()->RSSetState(States::rasterCullBack);
		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
	}

	void renderCubeAt(const DirectX::XMFLOAT3& location, const unsigned int& probeIndex)
	{
		const DirectX::XMVECTOR focusPoints[6] =
		{
			{1.0f,  0.0f,  0.0f},
			{-1.0f,  0.0f,  0.0f},
			{0.0f,  1.0f,  0.0f},
			{0.0f, -1.0f,  0.0f},
			{0.0f,  0.0f,  1.0f},
			{0.0f,  0.0f, -1.0f}
		};
		const DirectX::XMVECTOR upVectors[6] =
		{
			{0.0f, 1.0f,  0.0f},
			{0.0f, 1.0f,  0.0f},
			{0.0f,  0.0f,  -1.0f},
			{0.0f,  0.0f, 1.0f},
			{0.0f, 1.0f,  0.0f},
			{0.0f, 1.0f,  0.0f}
		};

		const DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(Math::pi / 2.f, 1.f, 0.1f, 512.f);

		for (int i = 0; i < 6; i++)
		{
			const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&location), DirectX::XMVectorAdd(focusPoints[i], DirectX::XMLoadFloat3(&location)), upVectors[i]);
			cubeProj.viewProj[i] = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);
		}

		cubeProj.probeLocation = location;
		cubeProj.probeIndex = probeIndex;

		memcpy(cubeProjBuffer->map(0).pData, &cubeProj, sizeof(CubeProj));
		cubeProjBuffer->unmap(0);

		renderCube->clearRTV(DirectX::Colors::Black);
		depthCube->clearDSV(D3D11_CLEAR_DEPTH);


		RenderAPI::get()->RSSetViewport(renderCube->getWidth(), renderCube->getHeight());
		RenderAPI::get()->OMSetRTV({ renderCube }, depthCube);
		RenderAPI::get()->VSSetConstantBuffer({ cubeProjBuffer }, 2);
		RenderAPI::get()->PSSetSRV({ globalShadowTexture }, 2);
		RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,lightProjBuffer,cubeProjBuffer }, 1);

		scene->renderCube(cubeRenderVS, cubeRenderPS);
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);

		if (Keyboard::getKeyDown(Keyboard::Q))
		{
			sunAngle += dt;
			sunAngle = Math::clamp(sunAngle, Math::half_pi - 0.365f, Math::half_pi + 0.365f);
			updateShadow();
		}
		else if (Keyboard::getKeyDown(Keyboard::E))
		{
			sunAngle -= dt;
			sunAngle = Math::clamp(sunAngle, Math::half_pi - 0.365f, Math::half_pi + 0.365f);
			updateShadow();
		}

		if (Keyboard::getKeyDown(Keyboard::Z))
		{
			exposure += 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::X))
		{
			exposure -= 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::N))
		{
			gamma += 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::M))
		{
			gamma -= 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
		}
	}

	void render()
	{
		RenderAPI::get()->RSSetState(States::rasterCullBack);

		RenderAPI::get()->GSSetShader(nullptr);
		RenderAPI::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

		depthTexture->clearDSV(D3D11_CLEAR_DEPTH);

		DirectX::XMFLOAT3 location;
		DirectX::XMStoreFloat3(&location, Camera::getEye());

		renderCubeAt(location, 0);

		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

		/*gBaseColor->clearRTV(DirectX::Colors::Black);
		gPosition->clearRTV(DirectX::Colors::Black);
		gNormalSpecular->clearRTV(DirectX::Colors::Black);

		RenderAPI::get()->OMSetRTV({ gPosition,gNormalSpecular,gBaseColor }, shadowMap);

		scene->render(deferredVShader, deferredPShader);

		originTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);
		RenderAPI::get()->PSSetSRV({ gPosition,gNormalSpecular,gBaseColor,hbaoEffect.process(shadowMap->getSRV(), gNormalSpecular->getSRV()),shadowTexture }, 0);
		RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,lightProjBuffer }, 1);

		RenderAPI::fullScreenVS->use();
		deferredFinal->use();

		RenderAPI::get()->DrawQuad();

		ShaderResourceView* const bloomTextureSRV = bloomEffect.process(originTexture);

		RenderAPI::get()->OMSetBlendState(nullptr);

		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->PSSetSRV({ bloomTextureSRV }, 0);
		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();
		RenderAPI::get()->DrawQuad();*/

		RenderAPI::get()->OMSetDefRTV(depthTexture);

		if (showRadiance)
		{
			RenderAPI::get()->PSSetSRV({ renderCube }, 0);

			RenderAPI::skyboxVS->use();
			skyboxPShader->use();

			RenderAPI::get()->DrawCube();
		}
		else
		{
			RenderAPI::get()->CSSetUAV({ irradianceCoeff }, 0);
			RenderAPI::get()->CSSetSRV({ renderCube,irradianceSamples }, 0);
			RenderAPI::get()->CSSetConstantBuffer({ cubeProjBuffer }, 1);
			RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

			irradianceCompute->use();

			RenderAPI::get()->Dispatch(1, 1, 1);

			RenderAPI::get()->PSSetSRV({ irradianceCoeff }, 0);

			RenderAPI::skyboxVS->use();
			irradianceEvaluate->use();

			RenderAPI::get()->DrawCube();
		}

	}


};