#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Color.h>
#include<Aurora/Timer.h>
#include<Aurora/PostProcessing/FadeEffect.h>

#include"Objects.h"

class MyGame :public Game
{
public:

	PrimitiveBatch* pBatch;

	MyGame() :
		pBatch(PrimitiveBatch::create()),
		currentSkyColor{ 0.0f,0.0f,0.0f,1.0f },
		texture(RenderTexture::create(1920, 1080,DXGI_FORMAT_R8G8B8A8_UNORM)),
		effect(new FadeEffect(texture))
	{
		Star::active = &starActive;
		Star::pool = &starPool;
		Spark::active = &sparkActive;
		Spark::pool = &sparkPool;

		Mouse::addLeftDownEvent([] {
			std::cout << "Launch\n";
			(new Shell(getShellConfig(ShellType::random)))->launch(Mouse::getX(), Mouse::getY());
			});
	}

	~MyGame()
	{

		for (std::list<Spark*>::iterator it = sparkActive.begin(); it != sparkActive.end(); it++)
		{
			delete* it;
		}

		for (std::list<Spark*>::iterator it = sparkPool.begin(); it != sparkPool.end(); it++)
		{
			delete* it;
		}

		for (std::list<Star*>::iterator it = starActive.begin(); it != starActive.end(); it++)
		{
			delete* it;
		}

		for (std::list<Star*>::iterator it = starPool.begin(); it != starPool.end(); it++)
		{
			delete* it;
		}
		delete pBatch;
		delete texture;
		delete effect;
	}

	void update(const float& dt) override
	{
		const float scaledDT = dt * 1.5f;
		const float width = Graphics::getWidth();
		const float height = Graphics::getHeight();
		speed = 1000.f / 16.6667f * scaledDT;

		const float starDrag = 1.f - (1.f - Star::airDrag) * speed;
		const float starDragHeavy = 1.f - (1.f - Star::airDragHeavy) * speed;
		const float sparkDrag = 1.f - (1.f - Spark::airDrag) * speed;
		const float gAcc = scaledDT * GRAVITY;

		for (std::list<Star*>::iterator it = Star::active->begin(); it != Star::active->end();)
		{
			Star* const star = *it;
			star->life -= scaledDT;

			if (star->life <= 0)
			{
				Star::active->erase(it++);
				Star::returnInstance(star);
			}
			else
			{
				const float burnRate = powf(star->life / star->fullLife, 0.5f);
				const float burnRateInverse = 1.f - burnRate;

				star->preX = star->x;
				star->preY = star->y;

				star->x += star->speedX * speed;
				star->y += star->speedY * speed;

				if (!star->heavy)
				{
					star->speedX *= starDrag;
					star->speedY *= starDrag;
				}
				else
				{
					star->speedX *= starDragHeavy;
					star->speedY *= starDragHeavy;
				}

				star->speedY -= gAcc;

				if (star->spinRadius)
				{
					star->spinAngle += star->spinSpeed * speed;
					star->x += sinf(star->spinAngle) * star->spinRadius * speed;
					star->y += cosf(star->spinAngle) * star->spinRadius * speed;
				}

				if (star->sparkFreq)
				{
					star->sparkTimer -= scaledDT;
					while (star->sparkTimer < 0)
					{
						star->sparkTimer += star->sparkFreq * 0.75f + star->sparkFreq * burnRateInverse * 4.f;
						Spark::add(
							star->x,
							star->y,
							star->sparkColor,
							Random::Float() * Math::two_pi,
							Random::Float() * star->sparkSpeed * burnRate,
							star->sparkLife * 0.8f + Random::Float() * star->sparkLifeVariation * star->sparkLife
						);
					}
				}

				if (star->life < star->transitionTime)
				{
					if (star->haveSecondColor && !star->colorChanged)
					{
						star->colorChanged = true;
						star->color = star->secondColor;

						if (star->color == Color::Invisible)
						{
							star->sparkFreq = 0;
						}
					}

					if (star->strobe)
					{
						star->visible = ((int)floorf(star->life / star->strobeFreq) % 3 == 0);
					}
				}
				it++;
			}
		}

		for (std::list<Spark*>::iterator it = Spark::active->begin(); it != Spark::active->end();)
		{
			Spark* const spark = *it;
			spark->life -= scaledDT;
			if (spark->life <= 0)
			{
				Spark::active->erase(it++);
				Spark::returnInstance(spark);
			}
			else
			{
				spark->preX = spark->x;
				spark->preY = spark->y;
				spark->x += spark->speedX * speed;
				spark->y += spark->speedY * speed;
				spark->speedX *= sparkDrag;
				spark->speedY *= sparkDrag;
				spark->speedY -= gAcc;
				it++;
			}
		}
	}

	void render() override
	{
		Graphics::setBlendState(StateCommon::defBlendState.Get());

		texture->setMSAARTV();

		pBatch->begin();
		for (std::list<Star*>::iterator it = Star::active->begin(); it != Star::active->end(); it++)
		{
			Star* const star = *it;
			if (!(star->color == Color::Invisible) && star->visible)
			{
				pBatch->drawRoundCapLine(star->preX, star->preY, star->x, star->y, Star::drawWidth, star->color.r, star->color.g, star->color.b, star->color.a);
				pBatch->drawLine(star->x - star->speedX * 1.6f, star->y - star->speedY * 1.6f, star->x, star->y, 1, 1, 1, star->color.a);
			}
		}

		for (std::list<Spark*>::iterator it = Spark::active->begin(); it != Spark::active->end(); it++)
		{
			Spark* const spark = *it;
			pBatch->drawLine(spark->preX, spark->preY, spark->x, spark->y, spark->color.r, spark->color.g, spark->color.b);
		}
		pBatch->end();

		texture->resolve();

		colorSky();
		Graphics::setDefRTV();

		Shader::displayVShader->use();
		Shader::displayPShader->use();
		Graphics::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());
		texture->getTexture()->setSRV();

		Graphics::context->Draw(3, 0);

		effect->process();

	}

	void colorSky()
	{
		const float maxSkySaturation = 30.f / 255.f;
		const int maxStarCount = 500;
		int totalStarCount = 0;

		targetSkyColor.r = 0;
		targetSkyColor.g = 0;
		targetSkyColor.b = 0;

		for (std::list<Star*>::iterator it = Star::active->begin(); it != Star::active->end(); it++)
		{
			Star* const star = *it;
			totalStarCount++;
			targetSkyColor.r += star->color.r;
			targetSkyColor.g += star->color.g;
			targetSkyColor.b += star->color.b;
		}

		const float intensity = powf(fminf(1, (float)totalStarCount / maxStarCount), 0.3f);

		float maxColorComponent = 1.f / 255.f;

		if (targetSkyColor.r > maxColorComponent)
		{
			maxColorComponent = targetSkyColor.r;
		}

		if (targetSkyColor.g > maxColorComponent)
		{
			maxColorComponent = targetSkyColor.g;
		}

		if (targetSkyColor.b > maxColorComponent)
		{
			maxColorComponent = targetSkyColor.b;
		}

		targetSkyColor.r = targetSkyColor.r / maxColorComponent * maxSkySaturation * intensity;
		targetSkyColor.g = targetSkyColor.g / maxColorComponent * maxSkySaturation * intensity;
		targetSkyColor.b = targetSkyColor.b / maxColorComponent * maxSkySaturation * intensity;

		const float colorChange = 10;

		currentSkyColor.r += (targetSkyColor.r - currentSkyColor.r) / colorChange * speed;
		currentSkyColor.g += (targetSkyColor.g - currentSkyColor.g) / colorChange * speed;
		currentSkyColor.b += (targetSkyColor.b - currentSkyColor.b) / colorChange * speed;

		Graphics::clearDefRTV(currentSkyColor);
	}

	const float GRAVITY = 0.9f;

	Color currentSkyColor;

	Color targetSkyColor;

	float speed;

	Timer timer;

	std::list<Star*> starActive;

	std::list<Star*> starPool;

	std::list<Spark*> sparkActive;

	std::list<Spark*> sparkPool;

	RenderTexture* texture;

	FadeEffect* effect;

};