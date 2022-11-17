#pragma once

#ifndef _ORBITCAMERA_H_
#define _ORBITCAMERA_H_

#include<Aurora/Mouse.h>
#include<Aurora/Camera.h>
#include<Aurora/Math.h>

class OrbitCamera
{
public:

	OrbitCamera() = delete;

	OrbitCamera(const OrbitCamera&) = delete;

	void operator=(const OrbitCamera&) = delete;

	OrbitCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& up);

	void registerEvent();

	void applyInput(const float& dt);

	void rotateX(const float& angle);

private:

	DirectX::XMVECTOR eye;

	float curLength;

	float length;

	const DirectX::XMVECTOR up;

	const DirectX::XMVECTOR focus = { 0,0,0 };

};

#endif // !_ORBITCAMERA_H_
