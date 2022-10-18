#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#define NOMINMAX

#include<DirectXMath.h>
#include<DirectXCollision.h>

#include"Renderer.h"
#include"Keyboard.h"
#include"Mouse.h"
#include"Graphics.h"
#include"DX/Resource/Buffer.h"

class Camera
{
public:

	Camera(const Camera&) = delete;

	void operator=(const Camera&) = delete;

	static const DirectX::XMMATRIX& getProj();

	static const DirectX::XMMATRIX& getView();

	static Buffer* getProjBuffer();

	static Buffer* getViewBuffer();

	static void setProj(const float& fov, const float& aspectRatio, const float& zNear, const float& zFar);

	static void setView(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	static void setView(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up);

	static DirectX::XMFLOAT3 toViewSpace(const DirectX::XMFLOAT3& pos);

	static const float& getFov();

	static const float& getNearPlane();

	static const float& getFarPlane();

	static constexpr float epsilon = 0.01f;

private:

	friend class Aurora;

	friend class CascadedShadowMap;

	static Camera* instance;

	Camera();

	~Camera();

	//自动转置proj矩阵
	static void setProj(const DirectX::XMMATRIX& proj);

	//自动转置view矩阵
	static void setView(const DirectX::XMMATRIX& view);

	Buffer* projBuffer;

	Buffer* viewBuffer;

	DirectX::XMMATRIX projMatrix;

	DirectX::XMMATRIX viewMatrix;

	float fov;

	float nearPlane;

	float farPlane;

	struct ViewInfo
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX normalMatrix;//light in view space inverseView
		DirectX::XMFLOAT4 eyePos;//light in world space
	}viewInfo;

};

#endif // !_CAMERA_H_
