#pragma once

#include<Aurora/EngineAPI/RenderPass.h>
#include<Aurora/EngineAPI/ResourceEssential.h>
#include<Aurora/EngineAPI/Camera.h>
#include<Aurora/EngineAPI/Graphics.h>

#include"Scene.h"

class LightProbeUpdatePass :public RenderPass
{
public:

	static constexpr UINT captureResolution = 64;

	LightProbeUpdatePass():
		cubeRenderVS(new Shader(Utils::getRootFolder() + "CubeRenderVS.cso", ShaderType::Vertex)),
		cubeRenderBouncePS(new Shader(Utils::getRootFolder() + "CubeRenderBouncePS.cso", ShaderType::Pixel)),
		cubeRenderPS(new Shader(Utils::getRootFolder() + "CubeRenderPS.cso", ShaderType::Pixel)),
		irradianceCompute(new Shader(Utils::getRootFolder() + "IrradianceCompute.cso", ShaderType::Compute)),
		octahedralEncode(new Shader(Utils::getRootFolder() + "OctahedralEncode.cso", ShaderType::Compute)),
		radianceCube(new RenderCube(captureResolution, FMT::RGBA16F)),
		distanceCube(new RenderCube(captureResolution, FMT::R32F)),
		depthCube(new DepthCube(captureResolution))
	{
		//为对应的位置计算投影矩阵
		{
			struct CubeRenderParam
			{
				DirectX::XMMATRIX viewProj[6];
				DirectX::XMFLOAT3 probeLocation;
				UINT probeIndex;
			} cubeRenderParam;

			for (UINT x = 0; x < irradianceVolumeParam.count.x; x++)
			{
				for (UINT z = 0; z < irradianceVolumeParam.count.z; z++)
				{
					for (UINT y = 0; y < irradianceVolumeParam.count.y; y++)
					{
						const DirectX::XMFLOAT3 location = ProbeGridPosToLoc({ x,y,z });

						const UINT probeIndex = ProbeGridPosToIndex({ x,y,z });

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

						cubeRenderBuffer[probeIndex] = new ConstantBuffer(sizeof(CubeRenderParam), D3D11_USAGE_IMMUTABLE, &cubeRenderParam);
					}
				}
			}
		}

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

			irradianceSamples = new StructuredBuffer((UINT)(sizeof(Sample) * samples.size()), sizeof(Sample), D3D11_USAGE_IMMUTABLE, samples.data());
		}

		irradianceCoeff = new ComputeTexture(9, 1, FMT::RG11B10F, FMT::RG11B10F, FMT::RG11B10F, 1, irradianceVolumeParam.count.x * irradianceVolumeParam.count.y * irradianceVolumeParam.count.z);

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

			GraphicsDevice::get()->createInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(cubeRenderVS), modelInputLayout.ReleaseAndGetAddressOf());
		}
	}

	~LightProbeUpdatePass()
	{
		delete scene;
		delete shadowTexture;
		delete radianceCube;
		delete distanceCube;
		delete depthCube;
		delete irradianceCoeff;
		delete irradianceBounceCoeff;
		delete depthOctahedralMap;
		delete irradianceSamples;
		delete cubeRenderVS;
		delete cubeRenderBouncePS;
		delete cubeRenderPS;
		delete irradianceCompute;
		delete octahedralEncode;

		for (UINT i = 0; i < 17 * 9 * 12; i++)
		{
			delete cubeRenderBuffer[i];
		}

	}

	Scene* scene;

	ComPtr<ID3D11InputLayout> modelInputLayout;

	ResourceDepthTexture* shadowTexture;

	RenderCube* radianceCube;

	RenderCube* distanceCube;

	DepthCube* depthCube;

	ComputeTexture* irradianceCoeff;

	ComputeTexture* irradianceBounceCoeff;

	ComputeTexture* depthOctahedralMap;

	Shader* cubeRenderVS;

	Shader* cubeRenderBouncePS;

	Shader* cubeRenderPS;

	Shader* irradianceCompute;

	Shader* octahedralEncode;

	ConstantBuffer* irradianceVolumeBuffer;

	ConstantBuffer* lightBuffer;

	ConstantBuffer* shadowProjBuffer;

	ConstantBuffer* cubeRenderBuffer[17 * 9 * 12];

	StructuredBuffer* irradianceSamples;

private:

	void recordCommand() override
	{
		context->OMSetBlendState(nullptr);
		context->RSSetState(States::rasterCullBack);
		context->OMSetDepthStencilState(States::defDepthStencilState, 0);

		updateLightProbe();
		updateLightBounceProbe();
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
		const UINT probeIndex = ProbeGridPosToIndex(probeGridPos);

		context->ClearRTV(radianceCube, DirectX::Colors::Black);
		float distanceClear[4] = { 512.f,512.f,512.f,512.f };
		context->ClearRTV(distanceCube, distanceClear);
		context->ClearDSV(depthCube, D3D11_CLEAR_DEPTH);

		context->OMSetBlendState(nullptr);
		context->IASetInputLayout(modelInputLayout.Get());
		context->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->RSSetViewport(captureResolution, captureResolution);
		context->OMSetRTV({ radianceCube,distanceCube }, depthCube);
		context->VSSetConstantBuffer({ cubeRenderBuffer[probeIndex] }, 2);
		context->PSSetSRV({ shadowTexture }, 3);
		context->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,shadowProjBuffer,cubeRenderBuffer[probeIndex] }, 1);
		context->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

		scene->renderCube(context, cubeRenderVS, cubeRenderPS);

		context->CSSetUAV({ irradianceCoeff->getMip(0) }, 0);
		context->CSSetSRV({ radianceCube,irradianceSamples }, 0);
		context->CSSetConstantBuffer({ cubeRenderBuffer[probeIndex] }, 1);
		context->CSSetSampler({ States::linearClampSampler }, 0);

		context->BindShader(irradianceCompute);

		context->Dispatch(1, 1, 1);

		context->CSSetUAV({ depthOctahedralMap->getMip(0) }, 0);
		context->CSSetSRV({ distanceCube }, 0);
		context->CSSetConstantBuffer({ cubeRenderBuffer[probeIndex] }, 1);
		context->CSSetSampler({ States::linearClampSampler }, 0);

		context->BindShader(octahedralEncode);

		context->Dispatch(1, 1, 1);
	}

	void RenderCubeBounceAt(const DirectX::XMUINT3& probeGridPos)
	{
		const UINT probeIndex = ProbeGridPosToIndex(probeGridPos);

		context->ClearRTV(radianceCube, DirectX::Colors::Black);
		context->ClearDSV(depthCube, D3D11_CLEAR_DEPTH);

		context->OMSetBlendState(nullptr);
		context->IASetInputLayout(modelInputLayout.Get());
		context->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->RSSetViewport(captureResolution, captureResolution);
		context->OMSetRTV({ radianceCube }, depthCube);
		context->VSSetConstantBuffer({ cubeRenderBuffer[probeIndex] }, 2);
		context->PSSetSRV({ shadowTexture,irradianceCoeff,depthOctahedralMap }, 3);
		context->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,shadowProjBuffer,cubeRenderBuffer[probeIndex],irradianceVolumeBuffer }, 1);
		context->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

		scene->renderCube(context, cubeRenderVS, cubeRenderBouncePS);

		context->CSSetUAV({ irradianceBounceCoeff->getMip(0) }, 0);
		context->CSSetSRV({ radianceCube,irradianceSamples }, 0);
		context->CSSetConstantBuffer({ cubeRenderBuffer[probeIndex] }, 1);
		context->CSSetSampler({ States::linearClampSampler }, 0);

		context->BindShader(irradianceCompute);

		context->Dispatch(1, 1, 1);
	}

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

};