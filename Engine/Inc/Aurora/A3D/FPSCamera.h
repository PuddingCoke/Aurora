#pragma once

#ifndef _FPSCAMERA_H_
#define _FPSCAMERA_H_

#include<Aurora/Mouse.h>
#include<Aurora/Keyboard.h>
#include<Aurora/Camera.h>

class FPSCamera
{
public:

	FPSCamera() = delete;

	FPSCamera(const FPSCamera&) = delete;

	void operator=(const FPSCamera&) = delete;

	FPSCamera(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& lookDir, const DirectX::XMFLOAT3& up, const float& moveSpeed, const float& rotationSpeed);

	void applyInput(const float& dt);

	void registerEvent();

private:

	const float moveSpeed;

	const float rotationSpeed;

	DirectX::XMFLOAT3 eye;

	DirectX::XMFLOAT3 lookDir;

	DirectX::XMFLOAT3 up;

};

#endif // !_FPSCAMERA_H_
