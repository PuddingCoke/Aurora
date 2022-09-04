#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include<DirectXMath.h>

#include"Renderer.h"
#include"Keyboard.h"
#include"Mouse.h"

class Camera
{
public:

	Camera() = delete;

	Camera(const Camera&) = delete;

	void operator=(const Camera&) = delete;

	static void setProj(const DirectX::XMMATRIX& proj);

	static void setProj(const float& fov, const float& aspectRatio, const float& zNear, const float& zFar);

	static void setView(const DirectX::XMMATRIX& view);

	static void setView(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	static void setView(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up);

	static DirectX::XMFLOAT3 toViewSpace(const DirectX::XMFLOAT3& pos);


private:

	friend class Aurora;

	static void initialize();

	static ComPtr<ID3D11Buffer> projBuffer;

	static ComPtr<ID3D11Buffer> viewBuffer;

	static DirectX::XMMATRIX viewMatrix;

	static struct ViewMatrices
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX normalMatrix;
	}viewMatrices;

};

#endif // !_CAMERA_H_
