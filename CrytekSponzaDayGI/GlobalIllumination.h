#pragma once

#include<Aurora/Resource/ComputeTexture.h>
#include<Aurora/Resource/StructuredBuffer.h>
#include<Aurora/Resource/RenderTexture.h>
#include<Aurora/Resource/ResourceDepthTexture.h>
#include<Aurora/Resource/RenderCube.h>
#include<Aurora/Resource/DepthCube.h>
#include<Aurora/Core/Shader.h>
#include<Aurora/EngineAPI/ImCtx.h>
#include<Aurora/Utils/Math.h>
#include<Aurora/Input/Keyboard.h>

#include<Aurora/Effect/HBAOEffect.h>
#include<Aurora/Effect/BloomEffect.h>
#include<Aurora/Effect/FXAAEffect.h>
#include<Aurora/Effect/SSREffect.h>

#include"Scene.h"

class GlobalIllumination
{
public:

	static constexpr UINT captureResolution = 64;

	static constexpr UINT shadowMapRes = 4096;

	static constexpr UINT skyboxResolution = 1024;

	struct IrradianceVolumeParam
	{
		DirectX::XMFLOAT3 start = { -142.f,-16.f,-74.f };
		float spacing = 18.2f;
		DirectX::XMUINT3 count = { 17,9,12 };
		float irradianceDistanceBias = 0.8f;
		float irradianceVarianceBias = 0.f;
		float irradianceChebyshevBias = -1.0f;
		DirectX::XMFLOAT2 padding;
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

	GlobalIllumination() :
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA32F, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA32F, DirectX::Colors::Black)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN, DirectX::Colors::Black)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F, DirectX::Colors::Black)),
		reflectedColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F, DirectX::Colors::Black)),
		depthTexture(new ResourceDepthTexture(Graphics::getWidth(), Graphics::getHeight())),
		shadowTexture(new ResourceDepthTexture(shadowMapRes, shadowMapRes)),
		radianceCube(new RenderCube(captureResolution, FMT::RGBA16F)),
		distanceCube(new RenderCube(captureResolution, FMT::R32F)),
		depthCube(new DepthCube(captureResolution)),
		skybox(new TextureCube(assetPath + "/sky/kloppenheim_05_4k.hdr", skyboxResolution)),
		deferredVShader(new Shader(Utils::getRootFolder() + "DeferredVShader.cso", ShaderType::Vertex)),
		deferredPShader(new Shader(Utils::getRootFolder() + "DeferredPShader.cso", ShaderType::Pixel)),
		deferredFinal(new Shader(Utils::getRootFolder() + "DeferredFinal.cso", ShaderType::Pixel)),
		skyboxPShader(new Shader(Utils::getRootFolder() + "SkyboxPShader.cso", ShaderType::Pixel)),
		cubeRenderVS(new Shader(Utils::getRootFolder() + "CubeRenderVS.cso", ShaderType::Vertex)),
		cubeRenderBouncePS(new Shader(Utils::getRootFolder() + "CubeRenderBouncePS.cso", ShaderType::Pixel)),
		cubeRenderPS(new Shader(Utils::getRootFolder() + "CubeRenderPS.cso", ShaderType::Pixel)),
		irradianceCompute(new Shader(Utils::getRootFolder() + "IrradianceCompute.cso", ShaderType::Compute)),
		octahedralEncode(new Shader(Utils::getRootFolder() + "OctahedralEncode.cso", ShaderType::Compute)),
		probeRenderVS(new Shader(Utils::getRootFolder() + "ProbeRenderVS.cso", ShaderType::Vertex)),
		probeRenderGS(new Shader(Utils::getRootFolder() + "ProbeRenderGS.cso", ShaderType::Geometry)),
		probeRenderPSDepth(new Shader(Utils::getRootFolder() + "ProbeRenderPSDepth.cso", ShaderType::Pixel)),
		probeRenderPSIrradiance(new Shader(Utils::getRootFolder() + "ProbeRenderPSIrradiance.cso", ShaderType::Pixel)),
		ssrCombineShader(new Shader(Utils::getRootFolder() + "SSRCombineShader.cso", ShaderType::Pixel)),
		cubeRenderBuffer(new ConstantBuffer(sizeof(CubeRenderParam), D3D11_USAGE_DYNAMIC)),
		lightBuffer(new ConstantBuffer(sizeof(Light), D3D11_USAGE_DYNAMIC)),
		shadowProjBuffer(new ConstantBuffer(sizeof(DirectX::XMMATRIX), D3D11_USAGE_DYNAMIC)),
		scene(Scene::create(assetPath + "/sponza.dae")),
		hbaoEffect(ImCtx::get(), Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(ImCtx::get(), Graphics::getWidth(), Graphics::getHeight()),
		fxaaEffect(ImCtx::get(), Graphics::getWidth(), Graphics::getHeight()),
		ssrEffect(ImCtx::get(), Graphics::getWidth(), Graphics::getHeight()),
		sunAngle(Math::half_pi - 0.01f),
		showIrradiance(true)
	{
		bloomEffect.setIntensity(0.5f);
		bloomEffect.applyChange();

		//预计算球面采样点
		{
			struct Sample
			{
				DirectX::XMFLOAT4 direction;
				float Ylm[9];
			};

			const unsigned int sampleCount = 256;
			const unsigned int sampleCountSqrt = 16;
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

			GraphicsDevice::get()->createInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(deferredVShader), modelInputLayout.ReleaseAndGetAddressOf());
		}

		Keyboard::addKeyDownEvent(Keyboard::K, [this]() {
			memcpy(ImCtx::get()->Map(irradianceVolumeBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &irradianceVolumeParam, sizeof(IrradianceVolumeParam));
			ImCtx::get()->Unmap(irradianceVolumeBuffer, 0);
			updateLightProbe();
			updateLightBounceProbe();
			});

		Keyboard::addKeyDownEvent(Keyboard::J, [this]() {
			showIrradiance = !showIrradiance;
			});

		irradianceCoeff = new ComputeTexture(9, 1, FMT::RG11B10F, FMT::RG11B10F, FMT::RG11B10F, 1, irradianceVolumeParam.count.x * irradianceVolumeParam.count.y * irradianceVolumeParam.count.z);
		irradianceBounceCoeff = new ComputeTexture(9, 1, FMT::RG11B10F, FMT::RG11B10F, FMT::RG11B10F, 1, irradianceVolumeParam.count.x * irradianceVolumeParam.count.y * irradianceVolumeParam.count.z);
		depthOctahedralMap = new ComputeTexture(16, 16, FMT::RG16F, FMT::RG16F, FMT::RG16F, 1, irradianceVolumeParam.count.x * irradianceVolumeParam.count.y * irradianceVolumeParam.count.z);

		irradianceVolumeBuffer = new ConstantBuffer(sizeof(IrradianceVolumeParam), D3D11_USAGE_DYNAMIC, &irradianceVolumeParam);

		updateShadow();
		updateLightProbe();
		updateLightBounceProbe();
	}

	~GlobalIllumination()
	{
		delete shadowTexture;
		delete depthTexture;
		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete originTexture;
		delete reflectedColor;
		delete irradianceCoeff;
		delete irradianceBounceCoeff;
		delete depthOctahedralMap;

		delete radianceCube;
		delete distanceCube;
		delete depthCube;
		delete skybox;

		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete skyboxPShader;
		delete cubeRenderVS;
		delete cubeRenderBouncePS;
		delete cubeRenderPS;
		delete irradianceCompute;
		delete octahedralEncode;
		delete probeRenderVS;
		delete probeRenderGS;
		delete probeRenderPSDepth;
		delete probeRenderPSIrradiance;
		delete ssrCombineShader;

		delete irradianceVolumeBuffer;
		delete cubeRenderBuffer;
		delete lightBuffer;
		delete shadowProjBuffer;
		delete irradianceSamples;

		delete scene;
	}

	void imGUICall()
	{
		ImGui::SliderFloat("irradiance distance bias", &irradianceVolumeParam.irradianceDistanceBias, -5.f, 5.f);
		ImGui::SliderFloat("irradiance variance bias", &irradianceVolumeParam.irradianceVarianceBias, -5.f, 5.f);
		ImGui::SliderFloat("irradiance chebyshev bias", &irradianceVolumeParam.irradianceChebyshevBias, -5.f, 5.f);
		bloomEffect.imGUIEffectModifier();
		fxaaEffect.imGUIEffectModifier();
	}

	void update(const float& dt)
	{
		if (Keyboard::getKeyDown(Keyboard::Q))
		{
			sunAngle += dt;
			sunAngle = Math::clamp(sunAngle, Math::half_pi - 0.365f, Math::half_pi + 0.365f);
			updateShadow();
			updateLightProbe();
			updateLightBounceProbe();
		}
		else if (Keyboard::getKeyDown(Keyboard::E))
		{
			sunAngle -= dt;
			sunAngle = Math::clamp(sunAngle, Math::half_pi - 0.365f, Math::half_pi + 0.365f);
			updateShadow();
			updateLightProbe();
			updateLightBounceProbe();
		}

	}

	void render()
	{
		ImCtx::get()->OMSetBlendState(nullptr);
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->RSSetState(States::rasterCullBack);
		ImCtx::get()->OMSetDepthStencilState(States::defDepthStencilState, 0);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		ImCtx::get()->IASetInputLayout(modelInputLayout.Get());

		ImCtx::get()->ClearDSV(depthTexture, D3D11_CLEAR_DEPTH);

		ImCtx::get()->ClearRTV(gBaseColor->getMip(0), DirectX::Colors::Transparent);
		ImCtx::get()->ClearRTV(gPosition->getMip(0), DirectX::Colors::Transparent);
		ImCtx::get()->ClearRTV(gNormalSpecular->getMip(0), DirectX::Colors::Transparent);

		ImCtx::get()->OMSetRTV({ gPosition->getMip(0),gNormalSpecular->getMip(0),gBaseColor->getMip(0) }, depthTexture);
		ImCtx::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

		scene->render(deferredVShader, deferredPShader);

		ImCtx::get()->ClearRTV(originTexture->getMip(0), DirectX::Colors::Black);
		ImCtx::get()->OMSetRTV({ originTexture->getMip(0) }, nullptr);
		ImCtx::get()->PSSetSRV({ gPosition,gNormalSpecular,gBaseColor,hbaoEffect.process(depthTexture->getSRV(), gNormalSpecular->getSRV()),shadowTexture,irradianceBounceCoeff,depthOctahedralMap }, 0);
		ImCtx::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,shadowProjBuffer,irradianceVolumeBuffer }, 1);
		ImCtx::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(deferredFinal);

		ImCtx::get()->DrawQuad();

		ImCtx::get()->OMSetRTV({ originTexture->getMip(0) }, depthTexture);

		ImCtx::get()->PSSetSRV({ skybox }, 0);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::skyboxVS);
		ImCtx::get()->BindShader(skyboxPShader);

		ImCtx::get()->DrawCube();

		ImCtx::get()->OMSetDefRTV(nullptr);

		ShaderResourceView* const uvVisibilitySRV = ssrEffect.process(depthTexture, gPosition, gNormalSpecular);

		ImCtx::get()->OMSetRTV({ reflectedColor->getMip(0) }, nullptr);
		ImCtx::get()->PSSetSRV({ originTexture,uvVisibilitySRV,gNormalSpecular }, 0);
		ImCtx::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(ssrCombineShader);

		ImCtx::get()->DrawQuad();

		ShaderResourceView* const bloomTextureSRV = bloomEffect.process(reflectedColor);

		ShaderResourceView* const antiAliasedSRV = fxaaEffect.process(bloomTextureSRV);

		ImCtx::get()->OMSetBlendState(nullptr);

		ImCtx::get()->OMSetDefRTV(nullptr);
		ImCtx::get()->PSSetSRV({ antiAliasedSRV }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(Shader::fullScreenPS);

		ImCtx::get()->DrawQuad();
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

		memcpy(ImCtx::get()->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &light, sizeof(Light));
		ImCtx::get()->Unmap(lightBuffer, 0);

		memcpy(ImCtx::get()->Map(shadowProjBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &lightMat, sizeof(DirectX::XMMATRIX));
		ImCtx::get()->Unmap(shadowProjBuffer, 0);

		ImCtx::get()->OMSetBlendState(nullptr);
		ImCtx::get()->IASetInputLayout(modelInputLayout.Get());

		ImCtx::get()->RSSetState(States::rasterShadow);
		ImCtx::get()->RSSetViewport(shadowMapRes, shadowMapRes);

		ImCtx::get()->ClearDSV(shadowTexture, D3D11_CLEAR_DEPTH);

		ImCtx::get()->OMSetRTV({}, shadowTexture);
		ImCtx::get()->VSSetConstantBuffer({ shadowProjBuffer }, 2);

		scene->renderGeometry(Shader::shadowVS);

		ImCtx::get()->RSSetState(States::rasterCullBack);
	}

	//光照方向改变
	void updateLightProbe()
	{
		for (UINT x = 0; x < irradianceVolumeParam.count.x; x++)
		{
			for (UINT z = 0; z < irradianceVolumeParam.count.z; z++)
			{
				for (UINT y = 0; y < irradianceVolumeParam.count.y; y++)
				{
					RenderCubeAt({ x,y,z });
				}
			}
		}
	}

	void updateLightBounceProbe()
	{
		for (UINT x = 0; x < irradianceVolumeParam.count.x; x++)
		{
			for (UINT z = 0; z < irradianceVolumeParam.count.z; z++)
			{
				for (UINT y = 0; y < irradianceVolumeParam.count.y; y++)
				{
					RenderCubeBounceAt({ x,y,z });
				}
			}
		}
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

		const DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(Math::pi / 2.f, 1.f, 1.f, 512.f);

		for (int i = 0; i < 6; i++)
		{
			const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&location), DirectX::XMVectorAdd(focusPoints[i], DirectX::XMLoadFloat3(&location)), upVectors[i]);
			cubeRenderParam.viewProj[i] = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);
		}

		cubeRenderParam.probeLocation = location;
		cubeRenderParam.probeIndex = probeIndex;

		memcpy(ImCtx::get()->Map(cubeRenderBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &cubeRenderParam, sizeof(CubeRenderParam));
		ImCtx::get()->Unmap(cubeRenderBuffer, 0);

		ImCtx::get()->ClearRTV(radianceCube, DirectX::Colors::Black);
		float distanceClear[4] = { 512.f,512.f,512.f,512.f };
		ImCtx::get()->ClearRTV(distanceCube, distanceClear);
		ImCtx::get()->ClearDSV(depthCube, D3D11_CLEAR_DEPTH);

		ImCtx::get()->OMSetBlendState(nullptr);
		ImCtx::get()->IASetInputLayout(modelInputLayout.Get());
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ImCtx::get()->RSSetViewport(captureResolution, captureResolution);
		ImCtx::get()->OMSetRTV({ radianceCube,distanceCube }, depthCube);
		ImCtx::get()->VSSetConstantBuffer({ cubeRenderBuffer }, 2);
		ImCtx::get()->PSSetSRV({ shadowTexture }, 3);
		ImCtx::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,shadowProjBuffer,cubeRenderBuffer }, 1);
		ImCtx::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

		scene->renderCube(cubeRenderVS, cubeRenderPS);

		ImCtx::get()->CSSetUAV({ irradianceCoeff->getMip(0) }, 0);
		ImCtx::get()->CSSetSRV({ radianceCube,irradianceSamples }, 0);
		ImCtx::get()->CSSetConstantBuffer({ cubeRenderBuffer }, 1);
		ImCtx::get()->CSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(irradianceCompute);

		ImCtx::get()->Dispatch(1, 1, 1);

		ImCtx::get()->CSSetUAV({ depthOctahedralMap->getMip(0) }, 0);
		ImCtx::get()->CSSetSRV({ distanceCube }, 0);
		ImCtx::get()->CSSetConstantBuffer({ cubeRenderBuffer }, 1);
		ImCtx::get()->CSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(octahedralEncode);

		ImCtx::get()->Dispatch(1, 1, 1);
	}

	void RenderCubeBounceAt(const DirectX::XMUINT3& probeGridPos)
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

		const DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(Math::pi / 2.f, 1.f, 1.f, 512.f);

		for (int i = 0; i < 6; i++)
		{
			const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&location), DirectX::XMVectorAdd(focusPoints[i], DirectX::XMLoadFloat3(&location)), upVectors[i]);
			cubeRenderParam.viewProj[i] = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);
		}

		cubeRenderParam.probeLocation = location;
		cubeRenderParam.probeIndex = probeIndex;

		memcpy(ImCtx::get()->Map(cubeRenderBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &cubeRenderParam, sizeof(CubeRenderParam));
		ImCtx::get()->Unmap(cubeRenderBuffer, 0);

		ImCtx::get()->ClearRTV(radianceCube, DirectX::Colors::Black);
		ImCtx::get()->ClearDSV(depthCube, D3D11_CLEAR_DEPTH);

		ImCtx::get()->OMSetBlendState(nullptr);
		ImCtx::get()->IASetInputLayout(modelInputLayout.Get());
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ImCtx::get()->RSSetViewport(captureResolution, captureResolution);
		ImCtx::get()->OMSetRTV({ radianceCube }, depthCube);
		ImCtx::get()->VSSetConstantBuffer({ cubeRenderBuffer }, 2);
		ImCtx::get()->PSSetSRV({ shadowTexture,irradianceCoeff,depthOctahedralMap }, 3);
		ImCtx::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,shadowProjBuffer,cubeRenderBuffer,irradianceVolumeBuffer }, 1);
		ImCtx::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

		scene->renderCube(cubeRenderVS, cubeRenderBouncePS);

		ImCtx::get()->CSSetUAV({ irradianceBounceCoeff->getMip(0) }, 0);
		ImCtx::get()->CSSetSRV({ radianceCube,irradianceSamples }, 0);
		ImCtx::get()->CSSetConstantBuffer({ cubeRenderBuffer }, 1);
		ImCtx::get()->CSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(irradianceCompute);

		ImCtx::get()->Dispatch(1, 1, 1);
	}

	ComPtr<ID3D11InputLayout> modelInputLayout;

	ResourceDepthTexture* shadowTexture;

	ResourceDepthTexture* depthTexture;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	RenderTexture* originTexture;

	RenderTexture* reflectedColor;

	ComputeTexture* irradianceCoeff;

	ComputeTexture* irradianceBounceCoeff;

	ComputeTexture* depthOctahedralMap;

	RenderCube* radianceCube;

	RenderCube* distanceCube;

	DepthCube* depthCube;

	TextureCube* skybox;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	Shader* skyboxPShader;

	Shader* cubeRenderVS;

	Shader* cubeRenderBouncePS;

	Shader* cubeRenderPS;

	Shader* irradianceCompute;

	Shader* octahedralEncode;

	Shader* probeRenderVS;

	Shader* probeRenderGS;

	Shader* probeRenderPSDepth;

	Shader* probeRenderPSIrradiance;

	Shader* ssrCombineShader;

	ConstantBuffer* irradianceVolumeBuffer;

	ConstantBuffer* cubeRenderBuffer;

	ConstantBuffer* lightBuffer;

	ConstantBuffer* shadowProjBuffer;

	StructuredBuffer* irradianceSamples;

	Scene* scene;

	float sunAngle;

	bool showIrradiance;

	HBAOEffect hbaoEffect;

	BloomEffect bloomEffect;

	FXAAEffect fxaaEffect;

	SSREffect ssrEffect;

};