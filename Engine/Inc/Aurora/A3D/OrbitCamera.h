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

	OrbitCamera(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& up);

	void registerEvent();

	void rotateX(const float& angle);

private:

	DirectX::XMVECTOR eye;

	const DirectX::XMVECTOR up;

	const DirectX::XMVECTOR focus = { 0,0,0 };

};

#endif // !_ORBITCAMERA_H_
