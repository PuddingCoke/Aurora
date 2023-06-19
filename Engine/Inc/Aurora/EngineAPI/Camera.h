#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#define NOMINMAX

#include<DirectXMath.h>

#include"Graphics.h"
#include"ImCtx.h"

#include<Aurora/Core/Renderer.h>
#include<Aurora/Input/Keyboard.h>
#include<Aurora/Input/Mouse.h>

#include<Aurora/Resource/ConstantBuffer.h>

class Camera
{
public:

	Camera(const Camera&) = delete;

	void operator=(const Camera&) = delete;

	static const DirectX::XMMATRIX& getProj();

	static const DirectX::XMMATRIX& getView();

	static ConstantBuffer* getProjBuffer();

	static ConstantBuffer* getViewBuffer();

	static const DirectX::XMVECTOR& getEye();

	static void setProj(const float& fov, const float& aspectRatio, const float& zNear, const float& zFar);

	static void setView(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up);

	static constexpr float epsilon = 0.01f;

private:

	friend class Aurora;

	static Camera* instance;

	Camera();

	~Camera();

	static void setProj(const DirectX::XMMATRIX& proj);

	static void setView(const DirectX::XMMATRIX& view);

	ConstantBuffer* projBuffer;

	ConstantBuffer* viewBuffer;

	DirectX::XMMATRIX projMatrix;

	DirectX::XMMATRIX viewMatrix;

	struct ViewInfo
	{
		DirectX::XMMATRIX view;
		DirectX::XMVECTOR eyePos;
		DirectX::XMMATRIX prevViewProj;
		DirectX::XMMATRIX viewProj;
		DirectX::XMMATRIX normalMatrix;
	}viewInfo;

};

#endif // !_CAMERA_H_
