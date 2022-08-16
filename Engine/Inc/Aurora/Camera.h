#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include<DirectXMath.h>

#include"Graphics.h"

class Camera
{
public:

	static void setProj(const DirectX::XMMATRIX& proj);

	static void setView(const DirectX::XMMATRIX& view);

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
