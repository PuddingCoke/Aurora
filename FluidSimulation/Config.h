#pragma once

#include<Aurora/Graphics.h>

#include<DirectXMath.h>
#include<cmath>

struct SimulationConfig
{
	static constexpr float COLOR_UPDATE_SPEED = 10;
	static constexpr float CURL = 100;
	static constexpr float DENSITY_DISSIPATION = 1;
	static constexpr float DYE_RESOLUTION = 1024;
	static constexpr float PRESSURE = 0.8f;
	static constexpr int PRESSURE_ITERATIONS = 20;
	static constexpr float SIM_RESOLUTION = 256;
	static constexpr float SPLAT_FORCE = 6000;
	static constexpr float SPLAT_RADIUS = 0.25f;
	static constexpr float SUNRAYS_RESOLUTION = 196;
	static constexpr float SUNRAYS_WEIGHT = 1;
	static constexpr float VELOCITY_DISSIPATION = 0.2f;
};

DirectX::XMINT2 getResolution(const float& resolution)
{
	int min = (int)roundf(resolution);
	int max = (int)roundf(resolution * Graphics::getAspectRatio());
	return DirectX::XMINT2(max, min);
}

//每帧更新一次
struct SplatParam
{
	DirectX::XMFLOAT2 mousePos;
	DirectX::XMFLOAT2 mouseDelta;
	DirectX::XMFLOAT4 color;
} splatParam;
