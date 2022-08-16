#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/A3D/Model.h>
#include<Aurora/DepthStencilView.h>

class MyGame :public Game
{
public:

	Model* models;

	unsigned int numModels;

	Shader* modelVShader;

	Shader* modelPShader;

	ComPtr<ID3D11InputLayout> inputLayout;

	DepthStencilView* depthStencilView;

	static constexpr DirectX::XMVECTORF32 up = { 0.f, 0.f, 1.f , 0.f };

	static constexpr DirectX::XMVECTORF32 focusPoint = { 0.f,0.f,0.f,1.f };

	float theta = 0.f;

	float theta2 = 0.f;

	float curRadius = 1.15f;

	float targetRadius = 1.15f;

	struct LightInfo
	{
		DirectX::XMFLOAT3 lightPos;
		float v0;
		DirectX::XMFLOAT3 viewPos;
		float v1;
	} lightInfo{};

	ComPtr<ID3D11Buffer> lightBuffer;

	static constexpr UINT boxSize = 512;

	static constexpr UINT prefilterSize = 128;

	static constexpr UINT irradianceSize = 32;

	Texture2D* envTexture;

	Shader* cubeVShader;

	Shader* cubePShader;

	ComPtr<ID3D11Texture2D> envCube;

	ComPtr<ID3D11ShaderResourceView> cubeSRV;

	ComPtr<ID3D11Texture2D> irradianceCube;

	ComPtr<ID3D11ShaderResourceView> irradianceSRV;

	ComPtr<ID3D11Texture2D> prefilterCube;

	ComPtr<ID3D11ShaderResourceView> prefilterSRV;

	Shader* bgVShader;

	Shader* bgPShader;

	Shader* irradiancePShader;

	Shader* prefilterPShader[5];

	Shader* brdfPShader;

	RenderTexture* brdfTexture;

	MyGame() :
		models(Model::create("DNA.obj", numModels)),
		modelVShader(Shader::fromFile("ModelVShader.hlsl", ShaderType::Vertex)),
		modelPShader(Shader::fromFile("ModelPShader.hlsl", ShaderType::Pixel)),
		depthStencilView(DepthStencilView::create(DXGI_FORMAT_D32_FLOAT)),
		envTexture(Texture2D::create("Tufts-Parking-Lot_Ref.hdr")),
		cubeVShader(Shader::fromFile("cubeVShader.hlsl", ShaderType::Vertex)),
		cubePShader(Shader::fromFile("cubePShader.hlsl", ShaderType::Pixel)),
		bgVShader(Shader::fromFile("BackgroundVShader.hlsl", ShaderType::Vertex)),
		bgPShader(Shader::fromFile("BackgroundPShader.hlsl", ShaderType::Pixel)),
		irradiancePShader(Shader::fromFile("IrradiancePShader.hlsl", ShaderType::Pixel)),
		brdfPShader(Shader::fromFile("BRDFPShader.hlsl", ShaderType::Pixel)),
		brdfTexture(RenderTexture::create(boxSize, boxSize, DXGI_FORMAT_R16G16_FLOAT, DirectX::Colors::Transparent, false))
	{
		prefilterPShader[0] = Shader::fromFile("PrefilterPShader0.hlsl", ShaderType::Pixel);
		prefilterPShader[1] = Shader::fromFile("PrefilterPShader1.hlsl", ShaderType::Pixel);
		prefilterPShader[2] = Shader::fromFile("PrefilterPShader2.hlsl", ShaderType::Pixel);
		prefilterPShader[3] = Shader::fromFile("PrefilterPShader3.hlsl", ShaderType::Pixel);
		prefilterPShader[4] = Shader::fromFile("PrefilterPShader4.hlsl", ShaderType::Pixel);

		{
			D3D11_INPUT_ELEMENT_DESC layout[3] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			Graphics::device->CreateInputLayout(layout, 3u, modelVShader->shaderBlob->GetBufferPointer(), modelVShader->shaderBlob->GetBufferSize(), inputLayout.ReleaseAndGetAddressOf());
		}

		{
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(LightInfo);
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			Graphics::device->CreateBuffer(&bd, nullptr, lightBuffer.ReleaseAndGetAddressOf());
		}

		{
			D3D11_TEXTURE2D_DESC tDesc = {};
			tDesc.Width = boxSize;
			tDesc.Height = boxSize;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
			tDesc.Format = envTexture->getFormat();
			tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			tDesc.ArraySize = 6;
			tDesc.MipLevels = 5;
			Graphics::device->CreateTexture2D(&tDesc, nullptr, envCube.ReleaseAndGetAddressOf());
		}

		{
			D3D11_TEXTURE2D_DESC tDesc = {};
			tDesc.Width = prefilterSize;
			tDesc.Height = prefilterSize;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			tDesc.Format = envTexture->getFormat();
			tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			tDesc.ArraySize = 6;
			tDesc.MipLevels = 5;
			Graphics::device->CreateTexture2D(&tDesc, nullptr, prefilterCube.ReleaseAndGetAddressOf());
		}

		{
			D3D11_TEXTURE2D_DESC tDesc = {};
			tDesc.Width = irradianceSize;
			tDesc.Height = irradianceSize;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			tDesc.Format = envTexture->getFormat();
			tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			tDesc.ArraySize = 6;
			tDesc.MipLevels = 1;
			Graphics::device->CreateTexture2D(&tDesc, nullptr, irradianceCube.ReleaseAndGetAddressOf());
		}

		{
			DirectX::XMMATRIX viewMatrices[6];
			{
				const DirectX::XMFLOAT4 eye = { 0,0,0,1 };
				const DirectX::XMFLOAT4 focusPoints[6] =
				{
					DirectX::XMFLOAT4(1.0f,  0.0f,  0.0f,1.0f),
					DirectX::XMFLOAT4(-1.0f,  0.0f,  0.0f,1.0f),
					DirectX::XMFLOAT4(0.0f,  1.0f,  0.0f,1.0f),
					DirectX::XMFLOAT4(0.0f, -1.0f,  0.0f,1.0f),
					DirectX::XMFLOAT4(0.0f,  0.0f,  1.0f,1.0f),
					DirectX::XMFLOAT4(0.0f,  0.0f, -1.0f,1.0f)
				};
				const DirectX::XMFLOAT4 upVectors[6] =
				{
					DirectX::XMFLOAT4(0.0f, 1.0f,  0.0f,1.0f),
					DirectX::XMFLOAT4(0.0f, 1.0f,  0.0f,1.0f),
					DirectX::XMFLOAT4(0.0f,  0.0f,  -1.0f,1.0f),
					DirectX::XMFLOAT4(0.0f,  0.0f, 1.0f,1.0f),
					DirectX::XMFLOAT4(0.0f, 1.0f,  0.0f,1.0f),
					DirectX::XMFLOAT4(0.0f, 1.0f,  0.0f,1.0f)
				};
				for (int i = 0; i < 6; i++)
				{
					viewMatrices[i] = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&eye), DirectX::XMLoadFloat4(&focusPoints[i]), DirectX::XMLoadFloat4(&upVectors[i]));
				}
			}

			D3D11_TEXTURE2D_DESC tDesc = {};
			tDesc.Format = envTexture->getFormat();
			tDesc.Width = boxSize;
			tDesc.Height = boxSize;
			tDesc.ArraySize = 1;
			tDesc.MipLevels = 1;
			tDesc.Usage = D3D11_USAGE_STAGING;
			tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;

			{

				ComPtr<ID3D11Texture2D> texture;

				Graphics::device->CreateTexture2D(&tDesc, nullptr, texture.ReleaseAndGetAddressOf());

				RenderTexture* renderTexture = RenderTexture::create(boxSize, boxSize, tDesc.Format, DirectX::Colors::Black, false);

				Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());
				envTexture->setSRV(0);

				cubeVShader->use();
				cubePShader->use();

				Graphics::setBlendState(StateCommon::defBlendState.Get());

				renderTexture->setRTV();

				Graphics::setViewport(boxSize, boxSize);

				Camera::setProj(DirectX::XMMatrixPerspectiveFovLH(Math::pi / 2.f, 1.f, 0.1f, 1000.f));

				for (int i = 0; i < 6; i++)
				{
					renderTexture->clearRTV(DirectX::Colors::Black);
					Camera::setView(viewMatrices[i]);
					Graphics::context->Draw(36, 0);
					Graphics::context->CopyResource(texture.Get(), renderTexture->getTexture()->getTexture2D());
					D3D11_MAPPED_SUBRESOURCE mappedData;
					Graphics::context->Map(texture.Get(), 0, D3D11_MAP_READ, 0, &mappedData);
					Graphics::context->UpdateSubresource(envCube.Get(), D3D11CalcSubresource(0, i, 5), 0,
						mappedData.pData, mappedData.RowPitch, mappedData.DepthPitch);
					Graphics::context->Unmap(texture.Get(), 0);
				}

				delete renderTexture;

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MipLevels = 5;
				srvDesc.TextureCube.MostDetailedMip = 0;

				Graphics::device->CreateShaderResourceView(envCube.Get(), &srvDesc, cubeSRV.GetAddressOf());

				Graphics::context->GenerateMips(cubeSRV.Get());

				renderTexture = RenderTexture::create(irradianceSize, irradianceSize, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false);

				renderTexture->setRTV();

				tDesc.Width = irradianceSize;
				tDesc.Height = irradianceSize;

				Graphics::device->CreateTexture2D(&tDesc, nullptr, texture.ReleaseAndGetAddressOf());

				Graphics::context->PSSetShaderResources(0, 1, cubeSRV.GetAddressOf());

				irradiancePShader->use();

				Graphics::setViewport(irradianceSize, irradianceSize);

				for (int i = 0; i < 6; i++)
				{
					renderTexture->clearRTV(DirectX::Colors::Black);
					Camera::setView(viewMatrices[i]);
					Graphics::context->Draw(36, 0);
					Graphics::context->CopyResource(texture.Get(), renderTexture->getTexture()->getTexture2D());
					D3D11_MAPPED_SUBRESOURCE mappedData;
					Graphics::context->Map(texture.Get(), 0, D3D11_MAP_READ, 0, &mappedData);
					Graphics::context->UpdateSubresource(irradianceCube.Get(), i, 0,
						mappedData.pData, mappedData.RowPitch, mappedData.DepthPitch);
					Graphics::context->Unmap(texture.Get(), 0);
				}

				delete renderTexture;

				srvDesc.Texture2D.MipLevels = 1;

				Graphics::device->CreateShaderResourceView(irradianceCube.Get(), &srvDesc, irradianceSRV.GetAddressOf());

			}

			RenderTexture* renderTextures[5];
			renderTextures[0] = RenderTexture::create(128, 128, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false);
			renderTextures[1] = RenderTexture::create(64, 64, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false);
			renderTextures[2] = RenderTexture::create(32, 32, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false);
			renderTextures[3] = RenderTexture::create(16, 16, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false);
			renderTextures[4] = RenderTexture::create(8, 8, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false);

			ComPtr<ID3D11Texture2D> textures[5];

			tDesc.Width = 128;
			tDesc.Height = 128;
			Graphics::device->CreateTexture2D(&tDesc, nullptr, textures[0].ReleaseAndGetAddressOf());

			tDesc.Width = 64;
			tDesc.Height = 64;
			Graphics::device->CreateTexture2D(&tDesc, nullptr, textures[1].ReleaseAndGetAddressOf());
			
			tDesc.Width = 32;
			tDesc.Height = 32;
			Graphics::device->CreateTexture2D(&tDesc, nullptr, textures[2].ReleaseAndGetAddressOf());

			tDesc.Width = 16;
			tDesc.Height = 16;
			Graphics::device->CreateTexture2D(&tDesc, nullptr, textures[3].ReleaseAndGetAddressOf());

			tDesc.Width = 8;
			tDesc.Height = 8;
			Graphics::device->CreateTexture2D(&tDesc, nullptr, textures[4].ReleaseAndGetAddressOf());

			Graphics::context->PSSetShaderResources(0, 1, cubeSRV.GetAddressOf());

			int resolution = prefilterSize;
			for (unsigned mip = 0; mip < 5; mip++)
			{
				Graphics::setViewport(prefilterSize, prefilterSize);
				renderTextures[mip]->setRTV();
				renderTextures[mip]->clearRTV(DirectX::Colors::Black);
				prefilterPShader[mip]->use();
				for (unsigned i = 0; i < 6; i++)
				{
					Camera::setView(viewMatrices[i]);
					Graphics::context->Draw(36, 0);
					Graphics::context->CopyResource(textures[mip].Get(), renderTextures[mip]->getTexture()->getTexture2D());
					D3D11_MAPPED_SUBRESOURCE mappedData;
					Graphics::context->Map(textures[mip].Get(), 0, D3D11_MAP_READ, 0, &mappedData);
					Graphics::context->UpdateSubresource(prefilterCube.Get(), D3D11CalcSubresource(mip, i, 5), 0,
						mappedData.pData, mappedData.RowPitch, mappedData.DepthPitch);
					Graphics::context->Unmap(textures[mip].Get(), 0);
				}

				resolution /= 2;
			}

			for (int i = 0; i < 5; i++)
			{
				delete renderTextures[i];
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels = 5;
			srvDesc.TextureCube.MostDetailedMip = 0;

			Graphics::device->CreateShaderResourceView(prefilterCube.Get(), &srvDesc, prefilterSRV.GetAddressOf());

			Graphics::setViewport(Graphics::getWidth(), Graphics::getHeight());
			Camera::setProj(DirectX::XMMatrixPerspectiveFovLH(Math::pi / 4.f, Graphics::getAspectRatio(), 0.1f, 1000.f));
		}

		{
			Graphics::setViewport(boxSize, boxSize);

			Shader::displayVShader->use();
			brdfPShader->use();

			brdfTexture->setRTV();

			Graphics::context->Draw(3, 0);

			Graphics::setViewport(Graphics::getWidth(), Graphics::getHeight());
		}

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					std::cout << theta << " " << theta2 << "\n";
					theta += Mouse::getDX() * 0.02f;
					theta2 -= Mouse::getDY() * 0.02f;
					theta2 = Math::clamp(theta2, -Math::half_pi + 0.1f, Math::half_pi - 0.1f);
				}
			});

		Mouse::addScrollEvent([this]()
			{
				targetRadius -= Mouse::getWheelDelta() * 0.5f;
				if (targetRadius < 0.1f)
				{
					targetRadius = 0.1f;
				}
			});
	}

	~MyGame()
	{
		delete[] models;
		delete modelVShader;
		delete modelPShader;
		delete depthStencilView;
		delete envTexture;
		delete cubePShader;
		delete cubeVShader;
		delete bgPShader;
		delete bgVShader;
		delete irradiancePShader;
		for (int i = 0; i < 5; i++)
		{
			delete prefilterPShader[i];
		}
		delete brdfPShader;
		delete brdfTexture;
	}

	void update(const float& dt) override
	{
		if ((curRadius - targetRadius) * (curRadius - targetRadius) > 0.0001f)
		{
			curRadius = Math::lerp(curRadius, targetRadius, 10.f * Graphics::getDeltaTime());
		}

		DirectX::XMFLOAT4 eyeRotated = DirectX::XMFLOAT4(curRadius * cosf(theta2) * cosf(theta), curRadius * cosf(theta2) * sinf(theta), curRadius * sinf(theta2), 1.f);

		Camera::setView(DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&eyeRotated), focusPoint, up));

		const float lightRadius = 1.15f;

		lightInfo.lightPos = DirectX::XMFLOAT3(lightRadius * cosf(theta2) * cosf(theta), lightRadius * cosf(theta2) * sinf(theta), lightRadius * sinf(theta2));

		lightInfo.lightPos = Camera::toViewSpace(lightInfo.lightPos);

		lightInfo.viewPos = DirectX::XMFLOAT3(eyeRotated.x, eyeRotated.y, eyeRotated.z);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		Graphics::context->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &lightInfo, sizeof(LightInfo));
		Graphics::context->Unmap(lightBuffer.Get(), 0);

		Graphics::context->PSSetConstantBuffers(3, 1, lightBuffer.GetAddressOf());
	}

	void render() override
	{
		depthStencilView->clear(D3D11_CLEAR_DEPTH);

		Graphics::clearDefRTV(DirectX::Colors::DimGray);

		Graphics::setDefRTV(depthStencilView->get());

		Graphics::setBlendState(StateCommon::defBlendState.Get());

		Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());

		Graphics::context->PSSetShaderResources(0, 1, cubeSRV.GetAddressOf());

		bgVShader->use();
		bgPShader->use();

		Graphics::context->Draw(36, 0);

		Graphics::context->PSSetShaderResources(0, 1, irradianceSRV.GetAddressOf());
		Graphics::context->PSSetShaderResources(1, 1, prefilterSRV.GetAddressOf());
		brdfTexture->getTexture()->setSRV(2);

		modelVShader->use();
		modelPShader->use();

		Graphics::context->IASetInputLayout(inputLayout.Get());

		Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());

		for (unsigned i = 0; i < numModels; i++)
		{
			models[i].draw();
		}

	}
};