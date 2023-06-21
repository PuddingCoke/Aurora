#pragma once

#include<Aurora/Game.h>
#include<Aurora/Resource/ComputeTexture.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComputeTexture* noiseTexture;

	ComputeTexture* copyTexture;

	MyGame() :
		noiseTexture(new ComputeTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN, FMT::RGBA8UN, FMT::RGBA8UN)),
		copyTexture(new ComputeTexture(*noiseTexture))
	{
	}

	~MyGame()
	{
		delete noiseTexture;
		delete copyTexture;
	}

	void update(const float& dt) override
	{

	}

	void render()
	{
		ImCtx::get()->GenNoise(noiseTexture->getMip(0), Graphics::getWidth(), Graphics::getHeight());

		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->OMSetBlendState(nullptr);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

		ImCtx::get()->PSSetSRV({ noiseTexture }, 0);
		ImCtx::get()->PSSetSampler({ States::pointClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(Shader::fullScreenPS);
		ImCtx::get()->DSUnbindShader();
		ImCtx::get()->HSUnbindShader();
		ImCtx::get()->GSUnbindShader();

		ImCtx::get()->DrawQuad();
	}


};