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

	struct ViewInfo
	{
		DirectX::XMMATRIX view;
		DirectX::XMFLOAT4 eyePos;
		DirectX::XMMATRIX prevViewProj;
	}viewInfo;

};

#endif // !_CAMERA_H_
