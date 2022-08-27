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

private:

	float originAngle;

	float deltaAngle;

	DirectX::XMFLOAT3 eye;

	const DirectX::XMFLOAT3 up;

	const DirectX::XMFLOAT3 focus = { 0,0,0 };

};

#endif // !_ORBITCAMERA_H_
