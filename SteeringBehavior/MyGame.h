#pragma once

#include<Aurora/Game.h>
#include<Aurora/DoubleRTV.h>
#include<Aurora/Random.h>
#include<Aurora/Math.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/PostProcessing/FadeEffect.h>
#include<Aurora/PostProcessing/BloomEffect.h>

#include<thread>

#include"Vehicle.h"

class MyGame :public Game
{
public:

	std::vector<Vehicle> vehicles;

	RenderTexture* renderTexture;

	DoubleRTV* doubleRTV;

	PrimitiveBatch* batch;

	FadeEffect fadeEffect;

	BloomEffect bloomEffect;

	float exposure;

	float gamma;

	MyGame() :
		batch(PrimitiveBatch::create()),
		renderTexture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black, true)),
		doubleRTV(DoubleRTV::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		fadeEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight())
	{
		bloomEffect.setExposure(2.05f);
		bloomEffect.setGamma(0.29f);
		bloomEffect.setThreshold(0);
		bloomEffect.applyChange();

		exposure = bloomEffect.getExposure();
		gamma = bloomEffect.getGamma();

		for (size_t i = 0; i < 1500; i++)
		{
			float angle = Random::Float() * Math::two_pi;
			float xSpeed = 3.f * cosf(angle);
			float ySpeed = 3.f * sinf(angle);
			vehicles.push_back(Vehicle(Vector(Random::Float() * Graphics::getWidth(), Random::Float() * Graphics::getHeight()), Vector(xSpeed, ySpeed), 2.f, 0.05f, Random::Float(), Random::Float(), Random::Float()));
		}

		Keyboard::addKeyDownEvent(Keyboard::B, [this]()
			{
				std::cout << exposure << "\n";
				std::cout << gamma << "\n";
			});
	}

	~MyGame()
	{
		delete batch;
		delete renderTexture;
		delete doubleRTV;
	}

	void update(const float& dt) override
	{
		if (Keyboard::getKeyDown(Keyboard::A))
		{
			exposure += 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::S))
		{
			exposure -= 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::H))
		{
			gamma += 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::J))
		{
			gamma -= 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
		}

		for (unsigned int i = 0; i < vehicles.size(); i++)
		{
			vehicles[i].flock(vehicles);
			vehicles[i].update(dt * 100.f);
		}
	}

	void render()
	{
		Renderer::setBlendState(States::defBlendState.Get());

		renderTexture->clearMSAARTV(DirectX::Colors::Black);
		renderTexture->setMSAARTV();

		batch->begin();
		for (unsigned int i = 0; i < vehicles.size(); i++)
		{
			batch->drawRoundCapLine(vehicles[i].prePos.x, vehicles[i].prePos.y, vehicles[i].pos.x, vehicles[i].pos.y, 4.f, vehicles[i].r, vehicles[i].g, vehicles[i].b);
			batch->drawRoundCapLine(vehicles[i].pos.x, vehicles[i].pos.y, vehicles[i].pos.x, vehicles[i].pos.y, 6.f, 1.f, 1.f, 1.f);
		}
		batch->end();

		renderTexture->resolve();

		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Renderer::setBlendState(States::addtiveBlend.Get());
		doubleRTV->write()->setRTV();

		Renderer::context->PSSetSamplers(0, 1, States::linearClampSampler.GetAddressOf());
		renderTexture->getTexture()->PSSetSRV(0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		Renderer::context->Draw(3, 0);
		doubleRTV->swap();

		Texture2D* bloomTexture = bloomEffect.process(doubleRTV->read()->getTexture());

		Renderer::clearDefRTV(DirectX::Colors::Black);
		Renderer::setDefRTV();
		bloomTexture->PSSetSRV(0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		Renderer::context->Draw(3, 0);

		Texture2D* const fadedTexture = fadeEffect.process(doubleRTV->read()->getTexture());

		doubleRTV->write()->setRTV();
		fadedTexture->PSSetSRV(0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		Renderer::context->Draw(3, 0);
	}


};