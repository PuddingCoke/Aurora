#pragma once

#include<fstream>

#include<Aurora/Game.h>
#include<Aurora/Core/PrimitiveBatch.h>
#include<Aurora/Resource/RenderTexture.h>

#include"Logo.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	RenderTexture* renderTexture;

	ResourceTexture* texture;

	MyGame() :
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN, DirectX::Colors::Black, true)),
		texture(new ResourceTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN, D3D11_USAGE_DEFAULT))
	{
		std::ifstream stream("info.txt");
		std::string text;
		std::getline(stream, text);
		stream.close();

		Logo logo(text);

		logo.setPos(-300, 300);

		PrimitiveBatch* pBatch = PrimitiveBatch::create();

		ImCtx::get()->ClearRTV(renderTexture->getMip(0), DirectX::Colors::White);
		ImCtx::get()->OMSetRTV({ renderTexture->getMip(0) }, nullptr);

		ImCtx::get()->OMSetBlendState(States::defBlendState);

		pBatch->begin();

		logo.draw();

		pBatch->end();

		ImCtx::get()->ResolveSubresource(texture, 0, renderTexture, 0, renderTexture->getFormat());

		delete pBatch;
	}

	~MyGame()
	{
		delete texture;
		delete renderTexture;
	}

	void update(const float& dt) override
	{

	}

	void render()
	{
		ImCtx::get()->ClearDefRTV(DirectX::Colors::White);
		ImCtx::get()->OMSetDefRTV(nullptr);

		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->OMSetBlendState(States::defBlendState);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(ImCtx::fullScreenVS);
		ImCtx::get()->BindShader(ImCtx::fullScreenPS);

		ImCtx::get()->PSSetSRV({ texture }, 0);
		ImCtx::get()->DrawQuad();
	}


};