#pragma once

#ifndef _ORBITCAMERA_H_
#define _ORBITCAMERA_H_

#include<Aurora/Input/Mouse.h>
#include<Aurora/Utils/Math.h>
#include<Aurora/EngineAPI/Camera.h>

class OrbitCamera
{
public:

	OrbitCamera() = delete;

	OrbitCamera(const OrbitCamera&) = delete;

	void operator=(const OrbitCamera&) = delete;

	OrbitCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& up, const float& scaleSpeed = 1.f);

	void registerEvent();

	void applyInput(const float& dt);

	void rotateX(const float& angle);

	void rotateY(const float& angle);

private:

	DirectX::XMVECTOR eye;

	float curLength;

	float length;

	const float scaleSpeed;

	const DirectX::XMVECTOR up;

};

#endif // !_ORBITCAMERA_H_
