#pragma once

#include<Aurora/ComputeTexture.h>
#include<Aurora/StructuredBuffer.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/ResDepthTexture.h>
#include<Aurora/A3D/RenderCube.h>
#include<Aurora/A3D/DepthCube.h>
#include<Aurora/Shader.h>
#include<Aurora/RenderAPI.h>
#include<Aurora/Math.h>
#include<Aurora/Keyboard.h>

#include<Aurora/PostProcessing/HBAOEffect.h>
#include<Aurora/PostProcessing/BloomEffect.h>

#include"Scene.h"

//伸展八面体贴图一点，防止采样边缘的问题

class GlobalIllumination
{
public:

	static constexpr UINT captureResolution = 128;

	static constexpr UINT shadowMapRes = 4096;

	struct IrradianceVolumeParam
	{
		DirectX::XMFLOAT3 start;
		float spacing;
		DirectX::XMUINT3 count;
		float padding;
	} irradianceVolumeParam;

	struct CubeRenderParam
	{
		DirectX::XMMATRIX viewProj[6];
		DirectX::XMFLOAT3 probeLocation;
		UINT probeIndex;
	} cubeRenderParam;

	struct Light
	{
		DirectX::XMVECTOR lightDir;
		DirectX::XMVECTOR lightColor;
	} light{};

	//16 8 8
	GlobalIllumination() :
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_SNORM, DirectX::Colors::Black)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		depthTexture(new ResDepthTexture(Graphics::getWidth(), Graphics::getHeight())),
		shadowTexture(new ResDepthTexture(shadowMapRes, shadowMapRes)),
		irradianceCoeff(new ComputeTexture(9, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, 1024)),
		depthOctahedralMap(new ComputeTexture(128, 128, DXGI_FORMAT_R16G16_FLOAT, 1024)),
		radianceCube(new RenderCube(captureResolution, DXGI_FORMAT_R16G16B16A16_FLOAT)),
		distanceCube(new RenderCube(captureResolution, DXGI_FORMAT_R32_FLOAT)),
		depthCube(new DepthCube(captureResolution)),
		deferredVShader(new Shader("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(new Shader("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(new Shader("DeferredFinal.hlsl", ShaderType::Pixel)),
		skyboxPShader(new Shader("SkyboxPShader.hlsl", ShaderType::Pixel)),
		cubeRenderVS(new Shader("CubeRenderVS.hlsl", ShaderType::Vertex)),
		cubeRenderPS(new Shader("CubeRenderPS.hlsl", ShaderType::Pixel)),
		irradianceCompute(new Shader("IrradianceCompute.hlsl", ShaderType::Compute)),
		octahedralEncode(new Shader("OctahedralEncode.hlsl", ShaderType::Compute)),
		irradianceVolumeBuffer(new Buffer(sizeof(IrradianceVolumeParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		cubeRenderBuffer(new Buffer(sizeof(CubeRenderParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		lightBuffer(new Buffer(sizeof(Light), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		shadowProjBuffer(new Buffer(sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		scene(Scene::create(assetPath + "/sponza.dae")),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		sunAngle(Math::half_pi - 0.02f)
	{
		//预计算球面采样点
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

					samples[i].direction = DirectX::XMFLOAT4((float)sin(theta) * (float)cos(phi), (float)sin(theta) * (float)sin(phi), (float)cos(theta), 1.0);

					for (int l = 0; l < 3; l++)
					{
						for (int m = -l; m <= l; m++)
						{
							int index = l * (l + 1) + m;
							samples[i].Ylm[index] = (float)SH(l, m, theta, phi);
						}
					}
					++i;
				}
			}

			irradianceSamples = new StructuredBuffer((UINT)(sizeof(Sample) * samples.size()), sizeof(Sample), D3D11_USAGE_IMMUTABLE, samples.data(), 0);
		}

		//初始化InputLayout
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
	}

	~GlobalIllumination()
	{
		delete shadowTexture;
		delete depthTexture;
		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete originTexture;
		delete irradianceCoeff;
		delete depthOctahedralMap;

		delete radianceCube;
		delete distanceCube;
		delete depthCube;

		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete skyboxPShader;
		delete cubeRenderVS;
		delete cubeRenderPS;
		delete irradianceCompute;
		delete octahedralEncode;

		delete irradianceVolumeBuffer;
		delete cubeRenderBuffer;
		delete lightBuffer;
		delete shadowProjBuffer;
		delete irradianceSamples;

		delete scene;
	}

	void update(const float& dt)
	{
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

		updateLightProbe();
	}

	//光照方向改变
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

		memcpy(shadowProjBuffer->map(0).pData, &lightMat, sizeof(DirectX::XMMATRIX));
		shadowProjBuffer->unmap(0);

		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());

		RenderAPI::get()->RSSetState(States::rasterShadow);
		RenderAPI::get()->RSSetViewport(shadowMapRes, shadowMapRes);

		shadowTexture->clearDSV(D3D11_CLEAR_DEPTH);

		RenderAPI::get()->OMSetRTV({}, shadowTexture);
		RenderAPI::get()->VSSetConstantBuffer({ shadowProjBuffer }, 2);

		scene->renderGeometry(RenderAPI::shadowVS);

		RenderAPI::get()->RSSetState(States::rasterCullBack);
		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
	}

	//光照方向改变
	void updateLightProbe()
	{

	}

	void debugDrawSphere()
	{

	}

private:

	double Fact(int n)
	{
		const double results[13] = { 1.0,1.0,2.0,6.0,24.0,120.0,720.0,5040.0,40320.0,362880.0,3628800.0,39916800.0,479001600.0 };
		return results[n];
	}

	double K(int l, int m)
	{
		double temp = ((2.0 * l + 1.0) * Fact(l - m)) / (4.0 * 3.141592653589793238 * Fact(l + m));
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

	UINT ProbeGridPosToIndex(const DirectX::XMUINT3& probeGridPos)
	{
		return probeGridPos.x + probeGridPos.z * irradianceVolumeParam.count.x + probeGridPos.y * irradianceVolumeParam.count.x * irradianceVolumeParam.count.z;
	}

	DirectX::XMFLOAT3 ProbeGridPosToLoc(const DirectX::XMUINT3& probeGridPos)
	{
		return {
			probeGridPos.x * irradianceVolumeParam.spacing + irradianceVolumeParam.start.x,
			probeGridPos.y * irradianceVolumeParam.spacing + irradianceVolumeParam.start.y,
			probeGridPos.z * irradianceVolumeParam.spacing + irradianceVolumeParam.start.z,
		};
	}

	void RenderCubeAt(const DirectX::XMUINT3& probeGridPos)
	{
		const DirectX::XMFLOAT3 location = ProbeGridPosToLoc(probeGridPos);

		const UINT probeIndex = ProbeGridPosToIndex(probeGridPos);

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
			cubeRenderParam.viewProj[i] = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);
		}

		cubeRenderParam.probeLocation = location;
		cubeRenderParam.probeIndex = probeIndex;

		memcpy(cubeRenderBuffer->map(0).pData, &cubeRenderParam, sizeof(CubeRenderParam));
		cubeRenderBuffer->unmap(0);

		radianceCube->clearRTV(DirectX::Colors::Black);
		float distanceClear[4] = { 512.f,512.f,512.f };
		distanceCube->clearRTV(distanceClear);

		depthCube->clearDSV(D3D11_CLEAR_DEPTH);

		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());

		RenderAPI::get()->RSSetViewport(captureResolution, captureResolution);
		RenderAPI::get()->OMSetRTV({ radianceCube,distanceCube }, depthCube);
		RenderAPI::get()->VSSetConstantBuffer({ cubeRenderBuffer }, 2);
		RenderAPI::get()->PSSetSRV({ shadowTexture }, 2);
		RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,shadowProjBuffer,cubeRenderBuffer }, 1);

		scene->renderCube(cubeRenderVS, cubeRenderPS);

		RenderAPI::get()->CSSetUAV({ irradianceCoeff }, 0);
		RenderAPI::get()->CSSetSRV({ radianceCube,irradianceSamples }, 0);
		RenderAPI::get()->CSSetConstantBuffer({ cubeRenderBuffer }, 1);
		RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

		irradianceCompute->use();

		RenderAPI::get()->Dispatch(1, 1, 1);

		RenderAPI::get()->CSSetUAV({ depthOctahedralMap }, 0);
		RenderAPI::get()->CSSetSRV({ distanceCube }, 0);
		RenderAPI::get()->CSSetConstantBuffer({ cubeRenderBuffer }, 1);
		RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

		octahedralEncode->use();

		RenderAPI::get()->Dispatch(128, 128, 1);
	}

	ComPtr<ID3D11InputLayout> inputLayout;

	ResDepthTexture* shadowTexture;

	ResDepthTexture* depthTexture;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	RenderTexture* originTexture;

	ComputeTexture* irradianceCoeff;

	ComputeTexture* depthOctahedralMap;

	RenderCube* radianceCube;

	RenderCube* distanceCube;

	DepthCube* depthCube;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	Shader* skyboxPShader;

	Shader* cubeRenderVS;

	Shader* cubeRenderPS;

	Shader* irradianceCompute;

	Shader* octahedralEncode;

	Buffer* irradianceVolumeBuffer;

	Buffer* cubeRenderBuffer;

	Buffer* lightBuffer;

	Buffer* shadowProjBuffer;

	StructuredBuffer* irradianceSamples;

	Scene* scene;

	float sunAngle;

	HBAOEffect hbaoEffect;

	BloomEffect bloomEffect;

};