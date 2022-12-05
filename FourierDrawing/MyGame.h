#pragma once

#pragma warning(disable : 4244)

#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/States.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Color.h>

#include"Epicycle.hpp"
#include"json.hpp"

using json = nlohmann::json;

class MyGame :public Game
{
public:

	const double tPi = 6.28318530717958647692;

	const double startX = 0;

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

	ResourceTexture* texture;

	const Color color{ 0.f,0.f,0.f };

	bool connected;

	MyGame() :
		sBatch(SpriteBatch::create()),
		pBatch(PrimitiveBatch::create()),
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Transparent, true)),
		texture(new ResourceTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_USAGE_DEFAULT)),
		curFrame(0),
		connected(false)
	{
		pBatch->setLineWidth(1.5f);
		pBatch->applyChange();

		const std::string filePath = "dft_data.json";
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
			epicycles[i].set(length - 1);
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
		delete texture;
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
		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::White);

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
			pBatch->drawCircle(preX, preY, epicycles[i].length, 0, 0, 0, 0.25f);
			pBatch->drawLine(preX, preY, x, y, 0, 0, 0, 0.25f);
		}
		pBatch->end();

		RenderAPI::get()->OMSetRTV({ renderTexture }, nullptr);
		pBatch->begin();
		if (curFrame++ == length)
		{
			if (connected)
			{
				pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, color.r, color.g, color.b);
			}
		}
		else
		{
			pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, color.r, color.g, color.b);
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
					pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, color.r, color.g, color.b);
				}
			}
			else
			{
				pBatch->drawRoundCapLine(lastX, lastY, x, y, 4.f, color.r, color.g, color.b);
			}

			lastX = x;
			lastY = y;

			t += 1.0;
		}

		pBatch->end();

		renderTexture->resolve(texture);

		RenderAPI::get()->OMSetDefRTV(nullptr);
		sBatch->begin();
		sBatch->draw(texture, texture, 0, 0);
		sBatch->end();

	}
};