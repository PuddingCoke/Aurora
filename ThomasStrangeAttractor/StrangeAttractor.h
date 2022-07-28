#pragma once

#include<Aurora/ParticleSystem.h>

class StrangeAttractor :public ParticleSystem
{
public:

	const unsigned int particleNum;

	//这个参数决定了粒子的明亮度
	static constexpr float colorFactor = 1.f;

	ComPtr<ID3D11InputLayout> inputLayout;

	ComPtr<ID3D11Buffer> particlePosBuffer;

	ComPtr<ID3D11UnorderedAccessView> uavView;

	ComPtr<ID3D11Buffer> particleColorBuffer;

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
		{
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = particleNum * sizeof(DirectX::XMFLOAT4);
			bd.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;

			D3D11_SUBRESOURCE_DATA subresource = {};
			subresource.pSysMem = positions;

			Graphics::device->CreateBuffer(&bd, &subresource, particlePosBuffer.ReleaseAndGetAddressOf());
		}

		//创建UAV Buffer
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.Flags = 0;
			uavDesc.Buffer.NumElements = particleNum;

			Graphics::device->CreateUnorderedAccessView(particlePosBuffer.Get(), &uavDesc, uavView.GetAddressOf());
		}

		//初始化粒子颜色信息
		{
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = particleNum * sizeof(DirectX::XMFLOAT4);
			bd.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA subresource = {};
			subresource.pSysMem = colors;

			Graphics::device->CreateBuffer(&bd, &subresource, particleColorBuffer.ReleaseAndGetAddressOf());
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

			Graphics::device->CreateInputLayout(layout, 2u, 
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

		Graphics::context->CSSetUnorderedAccessViews(1, 1, uavView.GetAddressOf(), nullptr);

		Graphics::context->Dispatch(particleNum / 1000u, 1, 1);

		Graphics::context->CSSetShader(nullptr, nullptr, 0);

		ID3D11UnorderedAccessView* nullView[1] = { nullptr };
		Graphics::context->CSSetUnorderedAccessViews(1, 1, nullView, nullptr);
	}

	void render() override
	{
		Graphics::setBlendState(StateCommon::addtiveBlend.Get());
		Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		
		ID3D11Buffer* buffers[2] = { particlePosBuffer.Get(),particleColorBuffer.Get() };

		const UINT stride[2] = { sizeof(DirectX::XMFLOAT4),sizeof(DirectX::XMFLOAT4) };
		const UINT offset[2] = { 0,0 };

		Graphics::context->IASetInputLayout(inputLayout.Get());

		Graphics::context->IASetVertexBuffers(0, 2, buffers, stride, offset);

		displayVShader->use();
		displayPShader->use();

		Graphics::context->Draw(particleNum, 0);

		ID3D11Buffer* nullBuffers[2] = { nullptr,nullptr };

		Graphics::context->IASetVertexBuffers(0, 2, nullBuffers, stride, offset);
	}


};