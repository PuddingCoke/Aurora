#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/States.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/PostProcessing/BloomEffect.h>

#include"Rain.h"

class MyGame :public Game
{
public:

	SpriteBatch* batch;

	BitmapFont* font;

	BloomEffect effect;

	std::vector<Rain> rains;

	RenderTexture* renderTexture;

	float exposure;

	float gamma;

	static constexpr float colorFactor = 4.5f;

	MyGame() :
		batch(SpriteBatch::create()),
		font(BitmapFont::create("Game_0.png", "Game.fnt", 24)),
		effect(Graphics::getWidth(), Graphics::getHeight()),
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black))
	{
		exposure = 0.64f;
		gamma = 0.56f;
		effect.setExposure(exposure);
		effect.setGamma(gamma);
		effect.applyChange();

		Rain::stride = font->getFontSize();

		for (size_t i = 0; i < Graphics::getWidth() / Rain::stride; i++)
		{
			rains.push_back(Rain((Graphics::getWidth() - Graphics::getWidth() / Rain::stride * Rain::stride) / 2 + i * Rain::stride, Random::Int() % 6 + 8));
		}
	}

	~MyGame()
	{
		delete batch;
		delete font;
		delete renderTexture;
	}

	void update(const float& dt) override
	{
		if (Keyboard::getKeyDown(Keyboard::A))
		{
			exposure += 0.01f;
			effect.setExposure(exposure);
			effect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::S))
		{
			exposure -= 0.01f;
			effect.setExposure(exposure);
			effect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::H))
		{
			gamma += 0.01f;
			effect.setGamma(gamma);
			effect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::J))
		{
			gamma -= 0.01f;
			effect.setGamma(gamma);
			effect.applyChange();
		}

		for (int i = 0; i < rains.size(); i++)
		{
			rains[i].update(dt);
			if (rains[i].y + Rain::stride * rains[i].len < 0)
			{
				rains[i].re();
			}
		}
	}

	void imGUICall() override
	{
		effect.imGUIEffectModifier();
	}

	void render() override
	{
		renderTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ renderTexture }, nullptr);
		RenderAPI::get()->OMSetBlendState(States::defBlendState);

		batch->begin();
		for (int i = 0; i < rains.size(); i++)
		{
			batch->draw(font, rains[i].character[0], rains[i].x, rains[i].y, colorFactor, colorFactor, colorFactor);
			for (int j = 1; j < rains[i].character.size(); j++)
			{
				batch->draw(font, rains[i].character[j], rains[i].x, rains[i].y + Rain::stride * j, 0.f, colorFactor * (1.f - (float)j / rains[i].character.size()), 0.f, 1.0f);
			}
		}
		batch->end();

		ShaderResourceView* bloomTextureSRV = effect.process(renderTexture);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->OMSetBlendState(States::defBlendState);
		RenderAPI::get()->PSSetSRV({ bloomTextureSRV }, 0);

		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();

	}
};