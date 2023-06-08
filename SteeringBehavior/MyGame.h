#pragma once

#include<Aurora/Game.h>
#include<Aurora/Resource/SwapTexture.h>
#include<Aurora/Utils/Random.h>
#include<Aurora/Utils/Math.h>
#include<Aurora/Effect/FadeEffect.h>
#include<Aurora/Effect/BloomEffect.h>

#include<thread>

#include"Vehicle.h"

class MyGame :public Game
{
public:

	std::vector<Vehicle> vehicles;

	RenderTexture* renderTexture;

	ResourceTexture* resolvedTexture;

	ResourceTexture* arrowTexture;

	SwapTexture<RenderTexture>* doubleRTV;

	FadeEffect fadeEffect;

	BloomEffect bloomEffect;

	float exposure;

	float gamma;

	MyGame() :
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black, true)),
		resolvedTexture(new ResourceTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_USAGE_DEFAULT)),
		arrowTexture(new ResourceTexture("arrow.png")),
		doubleRTV(new SwapTexture<RenderTexture>([] {return new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT); })),
		fadeEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight())
	{
		bloomEffect.setExposure(2.05f);
		bloomEffect.setGamma(0.29f);
		bloomEffect.setIntensity(0.97f);
		bloomEffect.setThreshold(0.0f);
		bloomEffect.applyChange();

		fadeEffect.setFadeSpeed(7.0f);

		exposure = bloomEffect.getExposure();
		gamma = bloomEffect.getGamma();

		for (size_t i = 0; i < 700; i++)
		{
			float angle = Random::Float() * Math::two_pi;
			float xSpeed = 3.f * cosf(angle);
			float ySpeed = 3.f * sinf(angle);
			vehicles.push_back(Vehicle(Vector(Random::Float() * Graphics::getWidth(), Random::Float() * Graphics::getHeight()), Vector(xSpeed, ySpeed), 3.f, 0.1f));
		}

		Keyboard::addKeyDownEvent(Keyboard::B, [this]()
			{
				std::cout << exposure << "\n";
				std::cout << gamma << "\n";
			});
	}

	~MyGame()
	{
		delete renderTexture;
		delete doubleRTV;
		delete resolvedTexture;
		delete arrowTexture;
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

			float length;

			DirectX::XMStoreFloat(&length, DirectX::XMVector2Length(DirectX::XMVectorSubtract({ vehicles[i].pos.x,vehicles[i].pos.y }, { Mouse::getX(),Mouse::getY() })));

			if (length < 50.f)
			{
				vehicles[i].apply(vehicles[i].flee(Vector(Mouse::getX(), Mouse::getY())) * 10.f);
			}

			vehicles[i].update(dt * 150.f);
		}
	}

	void render()
	{
		RenderAPI::get()->OMSetBlendState(States::defBlendState);

		renderTexture->clearRTV(DirectX::Colors::Black, 0);
		RenderAPI::get()->OMSetRTV({ renderTexture->getRTVMip(0) }, nullptr);

		/*batch->begin();
		for (size_t i = 0; i < vehicles.size(); i++)
		{
			batch->draw(TEXTURE(arrowTexture), vehicles[i].pos.x, vehicles[i].pos.y, arrowTexture->getWidth() / 2, arrowTexture->getHeight() / 2, atan2f(vehicles[i].vel.y, vehicles[i].vel.x));
		}
		batch->end();*/

		RenderAPI::get()->ResolveSubresource(resolvedTexture, 0, renderTexture, 0, renderTexture->getFormat());

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetBlendState(States::addtiveBlend);

		RenderAPI::get()->OMSetRTV({ doubleRTV->write()->getRTVMip(0) }, nullptr);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);
		RenderAPI::get()->PSSetSRV({ resolvedTexture }, 0);

		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();
		doubleRTV->swap();

		ShaderResourceView* bloomTextureSRV = bloomEffect.process(doubleRTV->read());

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->PSSetSRV({ bloomTextureSRV }, 0);

		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();

		ShaderResourceView* const fadedTextureSRV = fadeEffect.process(doubleRTV->read());

		RenderAPI::get()->OMSetRTV({ doubleRTV->write()->getRTVMip(0) }, nullptr);
		RenderAPI::get()->PSSetSRV({ fadedTextureSRV }, 0);

		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();
	}


};