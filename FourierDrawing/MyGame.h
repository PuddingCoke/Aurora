#pragma once

#pragma warning(disable : 4244)

#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/SpriteBatch.h>
#include<Aurora/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/PostProcessing/BlurEffect.h>

#include"Epicycle.hpp"
#include"json.hpp"

using json = nlohmann::json;

class MyGame :public Game
{
public:

	const double tPi = 6.28318530717958647692;

	const double startX = 100;

	const double startY = 0;

	double t = 0;

	double x;

	double y;

	double preX;

	double preY;

	Epicycle* epicycles;

	size_t length;

	double lastX, lastY;

	size_t curFrame;

	static constexpr int interval = 5;

	SpriteBatch* sBatch;

	PrimitiveBatch* pBatch;

	RenderTexture* renderTexture;

	bool connected;

	MyGame():
		sBatch(SpriteBatch::create()),
		pBatch(PrimitiveBatch::create()),
		renderTexture(RenderTexture::create(Graphics::getWidth(),Graphics::getHeight(),DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM)),
		curFrame(0),
		connected(false)
	{
		const std::string filePath = "maomao.json";
		std::ifstream ifs(filePath);
		json dftData = json::parse(ifs);
		ifs.close();
		
		length = dftData.size();

		std::cout << "Length is " << length << "\n";

		double real;
		double imag;
		double radius;

		epicycles = new Epicycle[length];

		const double scale = 1.0;

		for (size_t i = 0; i < length; i++)
		{
			real = dftData[i]["x"].get<double>() * scale;
			imag = dftData[i]["y"].get<double>() * scale;
			radius = sqrt(real * real + imag * imag);
			epicycles[i] = Epicycle(radius, (double)i * tPi / length, atan2(imag, real));
		}

		x = startX;
		y = startY;

		for (size_t i = 0; i < length; i++)
		{
			epicycles[i].set(t);
			preX = x;
			preY = y;
			x += epicycles[i].re;
			y += epicycles[i].im;
		}

		lastX = x;
		lastY = y;

		t += 1.0;

		curFrame++;

		x = startX;
		y = startY;

		for (size_t i = 0; i < length; i++)
		{
			epicycles[i].set(length-1);
			preX = x;
			preY = y;
			x += epicycles[i].re;
			y += epicycles[i].im;
		}

		if (sqrt((x - lastX) * (x - lastX) + (y - lastY) * (y - lastY)) < 50.0)
		{
			connected = true;
		}

	}

	~MyGame()
	{
		delete sBatch;
		delete pBatch;
		delete renderTexture;
		delete[] epicycles;
	}

	void update(const float& dt) override
	{

	}

	void render() override
	{
		Graphics::setDefRTV();
		Graphics::clearDefRTV(DirectX::Colors::White);

		x = startX;
		y = startY;

		pBatch->begin();
		for (size_t i = 0; i < length; i++)
		{
			epicycles[i].set(t);
			preX = x;
			preY = y;
			x += epicycles[i].re;
			y += epicycles[i].im;
			pBatch->drawCircle(preX, preY, epicycles[i].length, 0, 0, 0);
			pBatch->drawLine(preX, preY, x, y, 0, 0, 0);
		}
		pBatch->end();

		renderTexture->setMSAARTV();
		pBatch->begin();
		if (curFrame++ == length)
		{
			if (connected)
			{
				pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, 0, 0, 0);
			}
		}
		else
		{
			pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, 0, 0, 0);
		}
		
		lastX = x;
		lastY = y;

		t += 1.0;

		for (int intervalCount = 0; intervalCount < interval; intervalCount++)
		{
			x = startX;
			y = startY;

			for (size_t i = 0; i < length; i++)
			{
				epicycles[i].set(t);
				preX = x;
				preY = y;
				x += epicycles[i].re;
				y += epicycles[i].im;
			}

			if (curFrame++ == length)
			{
				if (connected)
				{
					pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, 0, 0, 0);
				}
			}
			else
			{
				pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, 0, 0, 0);
			}

			lastX = x;
			lastY = y;

			t += 1.0;
		}

		pBatch->end();

		renderTexture->resolve();

		Graphics::setDefRTV();
		sBatch->begin();
		sBatch->draw(renderTexture, 0, 0);
		sBatch->end();

	}
};