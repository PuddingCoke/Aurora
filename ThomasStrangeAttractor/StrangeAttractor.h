#pragma once

#include<Aurora/ParticleSystem.h>
#include<Aurora/Buffer.h>

class StrangeAttractor :public ParticleSystem
{
public:

	const unsigned int particleNum;

	//这个参数决定了粒子的明亮度
	static constexpr float colorFactor = 1.f;

	ComPtr<ID3D11InputLayout> inputLayout;

	std::shared_ptr<Buffer> particlePosBuffer;

	std::shared_ptr<Buffer> particleColorBuffer;

	ComPtr<ID3D11UnorderedAccessView> uavView;

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
				colors[i] = DirectX::XMFLOAT4(colorFactor, colorFactor, colorFactor, 1.f);
			}
			else
			{
				colors[i] = DirectX::XMFLOAT4(colorFactor, colorFactor, colorFactor * (radius * 0.4f + 0.1f), 1.f);
			}
		}

		//初始化粒子位置信息
		particlePosBuffer = std::shared_ptr<Buffer>(Buffer::create(
			particleNum*sizeof(DirectX::XMFLOAT4),
			D3D11_BIND_VERTEX_BUFFER|D3D11_BIND_UNORDERED_ACCESS,
			D3D11_USAGE_DEFAULT,
			positions));

		//初始化粒子颜色信息
		particleColorBuffer = std::shared_ptr<Buffer>(Buffer::create(
			particleNum * sizeof(DirectX::XMFLOAT4),
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE_IMMUTABLE,
			colors
		));

		//创建UAV Buffer
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.Flags = 0;
			uavDesc.Buffer.NumElements = particleNum;

			Renderer::device->CreateUnorderedAccessView(particlePosBuffer->get(), &uavDesc, uavView.GetAddressOf());
		}

		delete[] positions;
		delete[] colors;

		//初始化inputLayout
		{
			D3D11_INPUT_ELEMENT_DESC layout[2] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			Renderer::device->CreateInputLayout(layout, 2u, 
				displayVShader->shaderBlob->GetBufferPointer(), 
				displayVShader->shaderBlob->GetBufferSize(), 
				inputLayout.ReleaseAndGetAddressOf());
		}


	}

	~StrangeAttractor()
	{
		delete displayVShader;
		delete displayPShader;
		delete computeShader;
	}

	void compileShaders() override
	{
		displayVShader = Shader::fromFile("ParticleVertexShader.hlsl", ShaderType::Vertex);
		displayPShader = Shader::fromFile("ParticlePixelShader.hlsl", ShaderType::Pixel);
		computeShader = Shader::fromFile("ParticleComputeShader.hlsl", ShaderType::Compute);
	}

	void update(const float& dt) override
	{
		computeShader->use();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, uavView.GetAddressOf(), nullptr);

		Renderer::context->Dispatch(particleNum / 1000u, 1, 1);

		Renderer::context->CSSetShader(nullptr, nullptr, 0);

		ID3D11UnorderedAccessView* nullView[1] = { nullptr };
		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullView, nullptr);
	}

	void render() override
	{
		Renderer::setBlendState(StateCommon::addtiveBlend.Get());
		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		
		ID3D11Buffer* buffers[2] = { particlePosBuffer->get(),particleColorBuffer->get() };

		const UINT stride[2] = { sizeof(DirectX::XMFLOAT4),sizeof(DirectX::XMFLOAT4) };
		const UINT offset[2] = { 0,0 };

		Renderer::context->IASetInputLayout(inputLayout.Get());

		Renderer::context->IASetVertexBuffers(0, 2, buffers, stride, offset);

		displayVShader->use();
		displayPShader->use();

		Renderer::context->Draw(particleNum, 0);

		ID3D11Buffer* nullBuffers[2] = { nullptr,nullptr };

		Renderer::context->IASetVertexBuffers(0, 2, nullBuffers, stride, offset);
	}


};