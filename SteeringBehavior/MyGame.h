#pragma once

#include<Aurora/Game.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Random.h>
#include<Aurora/Math.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/PostProcessing/FadeEffect.h>

#include<thread>

#include"Vehicle.h"

class MyGame :public Game
{
public:

	std::vector<Vehicle> vehicles;

	RenderTexture* texture;

	PrimitiveBatch* batch;

	FadeEffect fadeEffect;

	MyGame() :
		batch(PrimitiveBatch::create()),
		texture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM)),
		fadeEffect(texture)
	{
		for (size_t i = 0; i < 1800; i++)
		{
			float angle = Random::Float() * Math::two_pi;
			float xSpeed = 3.f * cosf(angle);
			float ySpeed = 3.f * sinf(angle);
			vehicles.push_back(Vehicle(Vector(Random::Float() * Graphics::getWidth(), Random::Float() * Graphics::getHeight()), Vector(xSpeed, ySpeed), 2.f, 0.05f, Random::Float(), Random::Float(), Random::Float()));
		}
	}

	~MyGame()
	{
		delete batch;
		delete texture;
	}

	void update(const float& dt) override
	{
		for (unsigned int i = 0; i < vehicles.size(); i++)
		{
			vehicles[i].flock(vehicles);
			vehicles[i].update(dt * 100.f);
		}
	}

	void render()
	{
		Graphics::setBlendState(StateCommon::defBlendState.Get());

		texture->setMSAARTV();

		batch->begin();
		for (unsigned int i = 0; i < vehicles.size(); i++)
		{
			batch->drawRoundCapLine(vehicles[i].prePos.x, vehicles[i].prePos.y, vehicles[i].pos.x, vehicles[i].pos.y, 4.f, vehicles[i].r, vehicles[i].g, vehicles[i].b);
		}
		batch->end();

		texture->resolve();

		Graphics::clearDefRTV(DirectX::Colors::Black);
		Graphics::setDefRTV();

		Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());

		texture->getTexture()->setSRV(0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		Graphics::context->Draw(3, 0);

		fadeEffect.process();
	}


};