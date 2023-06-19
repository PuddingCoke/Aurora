#pragma once

#include<Aurora/Core/ParticleSystem.h>
#include<Aurora/Core/DX/Resource/Buffer.h>
#include<Aurora/Resource/ComputeVertexBuffer.h>

class StrangeAttractor :public ParticleSystem
{
public:

	const unsigned int particleNum;

	ComPtr<ID3D11InputLayout> inputLayout;

	ComputeVertexBuffer* particlePosBuffer;

	VertexBuffer* particleColorBuffer;

	Shader* displayVShader;

	Shader* displayPShader;

	Shader* computeShader;

	StrangeAttractor() = delete;

	StrangeAttractor(const unsigned int& particleNum) :
		particleNum(particleNum)
	{
		compileShaders();

		DirectX::XMFLOAT4* positions = new DirectX::XMFLOAT4[particleNum];
		DirectX::XMFLOAT4* colors = new DirectX::XMFLOAT4[particleNum];

		for (unsigned int i = 0; i < particleNum; i++)
		{
			const float radius = 0.3f * Random::Float() + 0.3f;
			const float theta = Random::Float() * Math::two_pi;
			const float theta2 = Random::Float() * Math::two_pi;

			positions[i] = DirectX::XMFLOAT4(radius * cosf(theta) * sinf(theta2), radius * cosf(theta) * cosf(theta2), radius * sinf(theta), 1.f);

			if (radius < 0.4f)
			{
				colors[i] = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
			}
			else
			{
				colors[i] = DirectX::XMFLOAT4(1.f, 1.f, radius * 0.4f + 0.1f, 1.f);
			}
		}

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = 0;
		uavDesc.Buffer.NumElements = particleNum;

		particlePosBuffer = new ComputeVertexBuffer(particleNum * sizeof(DirectX::XMFLOAT4), FMT::RGBA32F, positions);
		particleColorBuffer = new VertexBuffer(particleNum * sizeof(DirectX::XMFLOAT4), D3D11_USAGE_IMMUTABLE, colors);

		delete[] positions;
		delete[] colors;

		//初始化inputLayout
		{
			D3D11_INPUT_ELEMENT_DESC layout[2] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			Renderer::get()->createInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(displayVShader), inputLayout.ReleaseAndGetAddressOf());
		}


	}

	~StrangeAttractor()
	{
		delete particlePosBuffer;
		delete particleColorBuffer;
		delete displayVShader;
		delete displayPShader;
		delete computeShader;
	}

	void compileShaders() override
	{
		displayVShader = new Shader("ParticleVertexShader.hlsl", ShaderType::Vertex);
		displayPShader = new Shader("ParticlePixelShader.hlsl", ShaderType::Pixel);
		computeShader = new Shader("ParticleComputeShader.hlsl", ShaderType::Compute);
	}

	void update(const float& dt) override
	{
		ImCtx::get()->BindShader(computeShader);

		ImCtx::get()->CSSetUAV({ particlePosBuffer }, 0);

		ImCtx::get()->Dispatch(particleNum / 1000u, 1, 1);
	}

	void render() override
	{
		ImCtx::get()->OMSetBlendState(States::addtiveBlend);
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		ImCtx::get()->IASetInputLayout(inputLayout.Get());

		ImCtx::get()->IASetVertexBuffer(0, { particlePosBuffer,particleColorBuffer }, { sizeof(DirectX::XMFLOAT4),sizeof(DirectX::XMFLOAT4) }, { 0,0 });

		ImCtx::get()->BindShader(displayVShader);
		ImCtx::get()->BindShader(displayPShader);

		ImCtx::get()->Draw(particleNum, 0);
	}


};