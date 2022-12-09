#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A3D/OrbitCamera.h>
#include<Aurora/ComputeTexture.h>
#include<Aurora/StructuredBuffer.h>

#include"Scene.h"

const std::string assetPath = "D:/Assets/PBRRendering/";

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Scene scene;

	OrbitCamera camera;

	DepthStencilView* depthView;

	Buffer* lightBuffer;

	Shader* brdfGenPS;

	Shader* skyboxPS;

	RenderTexture* brdfTex;

	TextureCube* envCube;

	TextureCube* irradianceCube;

	TextureCube* prefilterCube;

	StructuredBuffer* irradianceSamples;

	Shader* irradianceCompute;

	Shader* irradianceEvaluate;

	ComputeTexture* irradianceCoeff;

	struct Light
	{
		DirectX::XMVECTOR lightPos;
		DirectX::XMFLOAT4 lightColor;
	}light;

	MyGame() :
		scene(assetPath + "DNA.obj"),
		depthView(new DepthStencilView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, true)),
		camera({ 2,0,0 }, { 0,1,0 }),
		lightBuffer(new Buffer(sizeof(Light), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		brdfGenPS(new Shader("BRDFGenPS.hlsl", ShaderType::Pixel)),
		skyboxPS(new Shader("SkyboxPS.hlsl", ShaderType::Pixel)),
		brdfTex(new RenderTexture(512, 512, DXGI_FORMAT_R32G32_FLOAT)),
		envCube(new TextureCube(assetPath + "ParkinglotEnvHDR.dds")),
		irradianceCube(new TextureCube(assetPath + "ParkinglotDiffuseHDR.dds")),
		prefilterCube(new TextureCube(assetPath + "ParkinglotSpecularHDR.dds")),
		irradianceCompute(new Shader("IrradianceCompute.hlsl", ShaderType::Compute)),
		irradianceEvaluate(new Shader("IrradianceEvaluate.hlsl", ShaderType::Pixel)),
		irradianceCoeff(new ComputeTexture(9, 1, DXGI_FORMAT_R11G11B10_FLOAT))
	{
		light.lightColor = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);

		RenderAPI::get()->RSSetViewport(512, 512);
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetRTV({ brdfTex }, nullptr);

		RenderAPI::fullScreenVS->use();
		brdfGenPS->use();

		RenderAPI::get()->DrawQuad();
		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

		camera.registerEvent();

		{
			struct Sample
			{
				DirectX::XMFLOAT4 direction;
				float Ylm[9];
			};

			const unsigned int sampleCount = 256;
			const unsigned int sampleCountSqrt = 16;
			const double oneoverN = 1.0 / (float)sampleCountSqrt;

			unsigned int i = 0;

			std::vector<Sample> samples(sampleCount);

			for (unsigned int a = 0; a < sampleCountSqrt; a++)
			{
				for (unsigned int b = 0; b < sampleCountSqrt; b++)
				{
					const double x = (a + Random::Double()) * oneoverN;
					const double y = (b + Random::Double()) * oneoverN;
					const double theta = 2.0 * acos(sqrt(1.0 - x));
					const double phi = 2.0 * 3.141592653589793 * y;

					samples[i].direction = DirectX::XMFLOAT4(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta), 1.0);

					for (int l = 0; l < 3; l++)
					{
						for (int m = -l; m <= l; m++)
						{
							int index = l * (l + 1) + m;
							samples[i].Ylm[index] = SH(l, m, theta, phi);
						}
					}

					i++;
				}
			}

			irradianceSamples = new StructuredBuffer(sizeof(Sample) * samples.size(), sizeof(Sample), D3D11_USAGE_IMMUTABLE, samples.data(), 0);
		}

	}

	~MyGame()
	{
		delete lightBuffer;
		delete depthView;
		delete brdfGenPS;
		delete skyboxPS;
		delete brdfTex;
		delete envCube;
		delete irradianceCube;
		delete prefilterCube;
		delete irradianceSamples;
		delete irradianceCompute;
		delete irradianceEvaluate;
		delete irradianceCoeff;
	}

	int factorial(int n)
	{
		int f = 1;

		for (int i = 1; i <= n; i++)
		{
			f *= i;
		}

		return f;
	}

	double K(int l, int m)
	{
		double temp = ((2.0 * l + 1.0) * factorial(l - m)) / (4.0 * 3.141592653589793 * factorial(l + m));
		return sqrt(temp);
	}

	double P(int l, int m, double x) {
		double pmm = 1.0;
		if (m > 0) {
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
		for (int ll = m + 2; ll <= l; ++ll) {
			pll = ((2.0 * ll - 1.0) * x * pmmp1 - (ll + m - 1.0) * pmm) / (ll - m);
			pmm = pmmp1;
			pmmp1 = pll;
		}

		return pll;
	}

	double SH(int l, int m, double theta, double phi) {
		const double sqrt2 = sqrt(2.0);
		if (m == 0)        return K(l, 0) * P(l, 0, cos(theta));
		else if (m > 0)    return sqrt2 * K(l, m) * cos(m * phi) * P(l, m, cos(theta));
		else                return sqrt2 * K(l, -m) * sin(-m * phi) * P(l, -m, cos(theta));
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);

		DirectX::XMVECTOR lightPos = Camera::getEye();
		lightPos = DirectX::XMVectorScale(DirectX::XMVector3Normalize(lightPos), 1.0f);

		light.lightPos = lightPos;

		memcpy(lightBuffer->map(0).pData, &light, sizeof(Light));
		lightBuffer->unmap(0);
	}

	void render()
	{
		RenderAPI::get()->CSSetUAV({ irradianceCoeff }, 0);
		RenderAPI::get()->CSSetSRV({ envCube,irradianceSamples }, 0);
		RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

		irradianceCompute->use();

		RenderAPI::get()->Dispatch(1, 1, 1);

		depthView->clear(D3D11_CLEAR_DEPTH);
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::CadetBlue);
		RenderAPI::get()->OMSetDefRTV(depthView);

		RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);
		RenderAPI::get()->PSSetConstantBuffer({ lightBuffer }, 3);
		RenderAPI::get()->PSSetSRV({ brdfTex,irradianceCube,prefilterCube,irradianceCoeff }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		scene.draw();

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->PSSetSRV({ irradianceCoeff }, 0);

		RenderAPI::skyboxVS->use();
		irradianceEvaluate->use();

		RenderAPI::get()->DrawCube();

		/*RenderAPI::get()->PSSetSRV({ envCube }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		RenderAPI::skyboxVS->use();
		skyboxPS->use();

		RenderAPI::get()->DrawCube();*/
	}


};