#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/PostProcessing/BloomEffect.h>

#include"StrangeAttractor.h"

//关于这种吸引子
//https://gereshes.com/2020/01/13/attracted-to-attractors/
class MyGame :public Game
{
public:

	StrangeAttractor attractor;

	float theta = 0.f;

	const float radius = 15.f;

	static constexpr DirectX::XMVECTORF32 up = { -1.f, -1.f, -1.f , 0.f };

	static constexpr DirectX::XMVECTORF32 focusPoint = { 0.f,0.f,0.f,1.f };

	static constexpr DirectX::XMVECTORF32 point = { 4.f,-4.f,-11.f,1.f };

	ComPtr<ID3D11DepthStencilView> depthStencilView;

	ComPtr<ID3D11Texture2D> depthStencilTexture;

	RenderTexture* renderTexture;

	BloomEffect bloomEffect;

	MyGame() :
		attractor(1000000),
		renderTexture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Transparent, false)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight())
	{
		{
			D3D11_TEXTURE2D_DESC tDesc = {};
			tDesc.Width = Graphics::getWidth();
			tDesc.Height = Graphics::getHeight();
			tDesc.MipLevels = 1;
			tDesc.ArraySize = 1;
			tDesc.Format = DXGI_FORMAT_D32_FLOAT;
			tDesc.SampleDesc.Count = 1;
			tDesc.SampleDesc.Quality = 0;
			tDesc.Usage = D3D11_USAGE_DEFAULT;
			tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			tDesc.CPUAccessFlags = 0;
			tDesc.MiscFlags = 0;

			Graphics::device->CreateTexture2D(&tDesc, nullptr, depthStencilTexture.ReleaseAndGetAddressOf());

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = tDesc.Format;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;

			Graphics::device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, depthStencilView.ReleaseAndGetAddressOf());
		}
	}

	~MyGame()
	{
		delete renderTexture;
	}

	void update(const float& dt) override
	{
		theta += dt;

		const DirectX::XMVECTOR rotPoint = DirectX::XMVector4Transform(point, DirectX::XMMatrixRotationAxis(up, theta));

		Graphics::setView(DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(rotPoint, focusPoint, up)));

		attractor.update(dt);
	}

	void render() override
	{
		Graphics::context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		renderTexture->clearRTV(DirectX::Colors::Transparent);
		renderTexture->setRTV(depthStencilView.Get());

		attractor.render();

		ID3D11RenderTargetView* view = nullptr;
		Graphics::context->OMSetRenderTargets(1, &view, nullptr);

		ID3D11ShaderResourceView* resourceViews[2] = { nullptr,nullptr };
		Graphics::context->PSSetShaderResources(0, 2, resourceViews);

		Texture2D* const texture = bloomEffect.process(renderTexture->getTexture());

		Graphics::setDefRTV();
		Graphics::clearDefRTV(DirectX::Colors::Black);

		Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());
		texture->setSRV(0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		Graphics::context->Draw(3, 0);

		Graphics::context->OMSetRenderTargets(1, &view, nullptr);
		Graphics::context->PSSetShaderResources(0, 2, resourceViews);

	}
};