#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/DX/View/DepthStencilView.h>

#include"Ocean.h"



class MyGame :public Game
{
public:

	static constexpr unsigned int skyDomeRes = 2048;

	static constexpr unsigned int skyDomeSliceCount = 50;

	static constexpr float cameraFarClip = 1000.f;

	FPSCamera camera;

	Ocean ocean;

	Shader* skyVS;

	Shader* skyPS;

	Shader* skyUpdatePS;

	Buffer* skyVertexBuffer;

	DepthStencilView* depthView;

	RenderTexture* skyTexture;

	TextureCube* textureCube;

	ComPtr<ID3D11InputLayout> inputLayout;

	MyGame() :
		camera({ -200,30,0 }, { 1,-0.07f,0 }, { 0,1,0 }, 50, 3),
		ocean(1024, 512, { 20.f,0.f }, 0.000004f),
		depthView(new DepthStencilView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, true)),
		skyVS(new Shader("SkyVS.hlsl", ShaderType::Vertex)),
		skyPS(new Shader("SkyPS.hlsl", ShaderType::Pixel)),
		skyUpdatePS(new Shader("SkyUpdatePS.hlsl", ShaderType::Pixel)),
		skyTexture(new RenderTexture(skyDomeRes, skyDomeRes, DXGI_FORMAT_R16G16B16A16_FLOAT)),
		textureCube(new TextureCube("ocean_env.dds"))
	{
		camera.registerEvent();

		{
			RenderAPI::get()->RSSetViewport(skyDomeRes, skyDomeRes);
			RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			RenderAPI::get()->OMSetBlendState(nullptr);
			skyTexture->clearRTV(DirectX::Colors::Black);
			RenderAPI::get()->OMSetRTV({ skyTexture }, nullptr);

			Shader::fullScreenVS->use();
			skyUpdatePS->use();

			RenderAPI::get()->DrawQuad();

			RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		}

		{
			D3D11_INPUT_ELEMENT_DESC layout[2] =
			{
				{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
				{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,16,D3D11_INPUT_PER_VERTEX_DATA,0}
			};

			Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(skyVS), inputLayout.ReleaseAndGetAddressOf());

			uint32_t skydome_vertices_count = skyDomeSliceCount * (skyDomeSliceCount + 2) * 2;

			float skyDomeRadius = cameraFarClip * 0.8f;

			float* skydome_vertexdata = new float[skydome_vertices_count * 6];

			for (int j = 0; j < skyDomeSliceCount; j++)
			{
				int i = 0;
				int floatnum = (j * (skyDomeSliceCount + 2) * 2) * 6;
				skydome_vertexdata[floatnum + 0] = skyDomeRadius * cos(2.0f * Math::pi * (float)i / (float)skyDomeSliceCount) * cos(-0.5f * Math::pi + Math::pi * (float)j / (float)skyDomeSliceCount);
				skydome_vertexdata[floatnum + 2] = skyDomeRadius * sin(-0.5f * Math::pi + Math::pi * (float)(j) / (float)skyDomeSliceCount);
				skydome_vertexdata[floatnum + 1] = skyDomeRadius * sin(2.0f * Math::pi * (float)i / (float)skyDomeSliceCount) * cos(-0.5f * Math::pi + Math::pi * (float)j / (float)skyDomeSliceCount);
				skydome_vertexdata[floatnum + 3] = 1.0f;
				skydome_vertexdata[floatnum + 4] = (float)i / (float)skyDomeSliceCount;
				skydome_vertexdata[floatnum + 5] = (float)(j + 0) / (float)skyDomeSliceCount;
				floatnum += 6;
				for (i = 0; i < skyDomeSliceCount + 1; i++)
				{
					skydome_vertexdata[floatnum + 0] = skyDomeRadius * cos(2.0f * Math::pi * (float)i / (float)skyDomeSliceCount) * cos(-0.5f * Math::pi + Math::pi * (float)j / (float)skyDomeSliceCount);
					skydome_vertexdata[floatnum + 2] = skyDomeRadius * sin(-0.5f * Math::pi + Math::pi * (float)(j) / (float)skyDomeSliceCount);
					skydome_vertexdata[floatnum + 1] = skyDomeRadius * sin(2.0f * Math::pi * (float)i / (float)skyDomeSliceCount) * cos(-0.5f * Math::pi + Math::pi * (float)j / (float)skyDomeSliceCount);
					skydome_vertexdata[floatnum + 3] = 1.0f;
					skydome_vertexdata[floatnum + 4] = (float)i / (float)skyDomeSliceCount;
					skydome_vertexdata[floatnum + 5] = 1.0f * (float)(j + 0) / (float)skyDomeSliceCount;
					floatnum += 6;
					skydome_vertexdata[floatnum + 0] = skyDomeRadius * cos(2.0f * Math::pi * (float)i / (float)skyDomeSliceCount) * cos(-0.5f * Math::pi + Math::pi * (float)(j + 1) / (float)skyDomeSliceCount);
					skydome_vertexdata[floatnum + 2] = skyDomeRadius * sin(-0.5f * Math::pi + Math::pi * (float)(j + 1) / (float)skyDomeSliceCount);
					skydome_vertexdata[floatnum + 1] = skyDomeRadius * sin(2.0f * Math::pi * (float)i / (float)skyDomeSliceCount) * cos(-0.5f * Math::pi + Math::pi * (float)(j + 1) / (float)skyDomeSliceCount);
					skydome_vertexdata[floatnum + 3] = 1.0f;
					skydome_vertexdata[floatnum + 4] = (float)i / (float)skyDomeSliceCount;
					skydome_vertexdata[floatnum + 5] = 1.0f * (float)(j + 1) / (float)skyDomeSliceCount;
					floatnum += 6;
				}
				i = skyDomeSliceCount;
				skydome_vertexdata[floatnum + 0] = skyDomeRadius * cos(2.0f * Math::pi * (float)i / (float)skyDomeSliceCount) * cos(-0.5f * Math::pi + Math::pi * (float)(j + 1) / (float)skyDomeSliceCount);
				skydome_vertexdata[floatnum + 2] = skyDomeRadius * sin(-0.5f * Math::pi + Math::pi * (float)(j + 1) / (float)skyDomeSliceCount);
				skydome_vertexdata[floatnum + 1] = skyDomeRadius * sin(2.0f * Math::pi * (float)i / (float)skyDomeSliceCount) * cos(-0.5f * Math::pi + Math::pi * (float)(j + 1) / (float)skyDomeSliceCount);
				skydome_vertexdata[floatnum + 3] = 1.0f;
				skydome_vertexdata[floatnum + 4] = (float)i / (float)skyDomeSliceCount;
				skydome_vertexdata[floatnum + 5] = 1.0f * (float)(j + 1) / (float)skyDomeSliceCount;
			}


			skyVertexBuffer = new Buffer(sizeof(float) * skydome_vertices_count * 6, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE, skydome_vertexdata);

			delete[] skydome_vertexdata;
		}

		Camera::setProj(Math::pi / 5.f, Graphics::getAspectRatio(), 1.f, cameraFarClip);
	}

	~MyGame()
	{
		delete depthView;
		delete skyUpdatePS;
		delete skyTexture;
		delete skyVS;
		delete skyPS;
		delete skyVertexBuffer;
		delete textureCube;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		ocean.update();
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::AliceBlue);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->RSSetState(States::get()->rasterCullNone.Get());
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());
		RenderAPI::get()->IASetVertexBuffer(0, { skyVertexBuffer }, { sizeof(float) * 6 }, { 0 });

		RenderAPI::get()->PSSetSRV({ skyTexture }, 0);
		RenderAPI::get()->PSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);

		skyVS->use();
		skyPS->use();
		RenderAPI::get()->HSSetShader(nullptr);
		RenderAPI::get()->DSSetShader(nullptr);

		RenderAPI::get()->Draw(skyDomeSliceCount * (skyDomeSliceCount + 2) * 2, 0);
		RenderAPI::get()->UnbindVertexBuffer();

		RenderAPI::get()->RSSetState(States::get()->rasterCullBack.Get());

		depthView->clear(D3D11_CLEAR_DEPTH);
		RenderAPI::get()->OMSetDefRTV(depthView);

		RenderAPI::get()->PSSetSRV({ textureCube }, 1);

		ocean.render();
	}


};