#pragma once

#include<Aurora/Graphics.h>

#include<DirectXMath.h>
#include<cmath>

struct SimulationConfig
{
	const float COLOR_UPDATE_SPEED = 10;
	const float CURL = 30;
	const float DENSITY_DISSIPATION = 1;
	const float DYE_RESOLUTION = 1024;
	const float PRESSURE = 0.8f;
	const int PRESSURE_ITERATIONS = 20;
	const float SIM_RESOLUTION = 256;
	const float SPLAT_FORCE = 6000;
	const float SPLAT_RADIUS = 0.25f;
	const float SUNRAYS_RESOLUTION = 196;
	const float SUNRAYS_WEIGHT = 1;
	const float VELOCITY_DISSIPATION = 0.2f;
} config;

DirectX::XMINT2 getResolution(const float& resolution)
{
	int min = (int)roundf(resolution);
	int max = (int)roundf(resolution * Graphics::getAspectRatio());
	return DirectX::XMINT2(max, min);
}

//每帧更新一次
struct BufferDynamic
{
	DirectX::XMFLOAT3 color0;
	float v0;
	DirectX::XMFLOAT3 color1;
	float v1;
	DirectX::XMFLOAT2 point;
	float v2[2];
} cBufferB2;
