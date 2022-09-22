#pragma once

#include<fstream>

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/RenderTexture.h>

#include"Logo.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	RenderTexture* renderTexture;

	MyGame() :
		renderTexture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black, true))
	{
		std::ifstream stream("info.txt");
		std::string text;
		std::getline(stream, text);
		stream.close();

		Logo logo(text);

		logo.setPos(-300, 300);

		PrimitiveBatch* pBatch = PrimitiveBatch::create();

		renderTexture->clearMSAARTV(DirectX::Colors::White);
		renderTexture->setMSAARTV();

		Renderer::setBlendState(StateCommon::defBlendState.Get());

		pBatch->begin();

		logo.draw();

		pBatch->end();

		renderTexture->resolve();

		delete pBatch;
	}

	~MyGame()
	{
		delete renderTexture;
	}

	void update(const float& dt) override
	{

	}

	void render()
	{
		Renderer::clearDefRTV(DirectX::Colors::White);
		Renderer::setDefRTV();

		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Renderer::setBlendState(StateCommon::defBlendState.Get());
		Renderer::setSampler(0, StateCommon::linearClampSampler.Get());

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		renderTexture->getTexture()->setSRV();

		Renderer::draw(3, 0);
	}


};