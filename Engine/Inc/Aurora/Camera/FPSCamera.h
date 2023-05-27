#pragma once

#ifndef _FPSCAMERA_H_
#define _FPSCAMERA_H_

#include<Aurora/Input/Mouse.h>
#include<Aurora/Input/Keyboard.h>
#include<Aurora/EngineAPI/Camera.h>
#include<Aurora/Utils/Math.h>

class FPSCamera
{
public:

	FPSCamera() = delete;

	FPSCamera(const FPSCamera&) = delete;

	void operator=(const FPSCamera&) = delete;

	FPSCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& lookDir, const DirectX::XMVECTOR& up, const float& moveSpeed);

	void applyInput(const float& dt);

	void registerEvent();

private:

	const float moveSpeed;

	DirectX::XMVECTOR eye;

	DirectX::XMVECTOR lookDir;

	DirectX::XMVECTOR up;

};

#endif // !_FPSCAMERA_H_
