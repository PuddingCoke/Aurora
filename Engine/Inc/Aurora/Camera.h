#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#define NOMINMAX
#define NONEAR
#define NOFAR

#include<DirectXMath.h>
#include<DirectXCollision.h>

#include"Renderer.h"
#include"Keyboard.h"
#include"Mouse.h"
#include"Graphics.h"

class Camera
{
public:

	Camera() = delete;

	Camera(const Camera&) = delete;

	void operator=(const Camera&) = delete;

	static const DirectX::XMMATRIX& getProj();

	static const DirectX::XMMATRIX& getView();

	static ID3D11Buffer* getProjBuffer();

	static ID3D11Buffer* getViewBuffer();

	static void setProj(const float& fov, const float& aspectRatio, const float& zNear, const float& zFar);

	static void setView(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	static void setView(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up);

	static DirectX::XMFLOAT3 toViewSpace(const DirectX::XMFLOAT3& pos);

	static const float& getFov();

	static const float& getNearPlane();

	static const float& getFarPlane();

	static constexpr float epsilon = 0.001f;

private:

	friend class Aurora;

	friend class CascadedShadowMap;

	static void initialize();

	//�Զ�ת��proj����
	static void setProj(const DirectX::XMMATRIX& proj);

	//�Զ�ת��view����
	static void setView(const DirectX::XMMATRIX& view);

	static ComPtr<ID3D11Buffer> projBuffer;

	static ComPtr<ID3D11Buffer> viewBuffer;

	//������������û��ת�õģ���Ϊ��DirectXMath�У��������������ȡ�
	static DirectX::XMMATRIX projMatrix;

	static DirectX::XMMATRIX viewMatrix;

	static float fov;

	static float nearPlane;

	static float farPlane;

	static struct ViewInfo
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX normalMatrix;//light in view space inverseView
		DirectX::XMFLOAT4 eyePos;//light in world space
	}viewInfo;

};

#endif // !_CAMERA_H_
