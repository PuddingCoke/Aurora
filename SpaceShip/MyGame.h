#pragma once

#include<Aurora/Game.h>
#include<Aurora/TextureCube.h>

class MyGame :public Game
{
public:

	DirectX::XMFLOAT3 eye = { 0,0,0 };
	DirectX::XMFLOAT3 lookDir = { 1,0,0 };
	DirectX::XMFLOAT3 up = { 0,1,0 };

	const float moveSpeed = 100.f;
	const float rotationSpeed = 5.f;

	MyGame()
	{
		Camera::setProj(DirectX::XMMatrixPerspectiveFovLH(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f));
		Camera::setView(DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&lookDir), DirectX::XMLoadFloat3(&up)));

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&up), Mouse::getDX() / 120.f);

					DirectX::XMStoreFloat3(&lookDir, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&lookDir), rotMat));

					const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&up), DirectX::XMLoadFloat3(&lookDir));

					const DirectX::XMMATRIX upRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, -Mouse::getDY() / 120.f);

					DirectX::XMStoreFloat3(&up, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&up), upRotMat));
					DirectX::XMStoreFloat3(&lookDir, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&lookDir), upRotMat));
				}
			});
	}

	~MyGame()
	{
	}

	void update(const float& dt) override
	{
		if (Keyboard::getKeyDown(Keyboard::W))
		{
			eye.x += moveSpeed * lookDir.x * dt;
			eye.y += moveSpeed * lookDir.y * dt;
			eye.z += moveSpeed * lookDir.z * dt;
		}
		else if (Keyboard::getKeyDown(Keyboard::S))
		{
			eye.x -= moveSpeed * lookDir.x * dt;
			eye.y -= moveSpeed * lookDir.y * dt;
			eye.z -= moveSpeed * lookDir.z * dt;
		}

		if (Keyboard::getKeyDown(Keyboard::A))
		{
			DirectX::XMFLOAT3 upCrossLookDir;
			DirectX::XMStoreFloat3(&upCrossLookDir, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&up), DirectX::XMLoadFloat3(&lookDir))));

			eye.x -= upCrossLookDir.x * moveSpeed * dt;
			eye.y -= upCrossLookDir.y * moveSpeed * dt;
			eye.z -= upCrossLookDir.z * moveSpeed * dt;

		}
		if (Keyboard::getKeyDown(Keyboard::D))
		{
			DirectX::XMFLOAT3 upCrossLookDir;
			DirectX::XMStoreFloat3(&upCrossLookDir, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&up), DirectX::XMLoadFloat3(&lookDir))));

			eye.x += upCrossLookDir.x * moveSpeed * dt;
			eye.y += upCrossLookDir.y * moveSpeed * dt;
			eye.z += upCrossLookDir.z * moveSpeed * dt;
		}

		if (Keyboard::getKeyDown(Keyboard::Q))
		{
			const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&lookDir), dt * rotationSpeed);

			DirectX::XMStoreFloat3(&up, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&up), rotMat));

		}

		if (Keyboard::getKeyDown(Keyboard::E))
		{
			const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&lookDir), -dt * rotationSpeed);

			DirectX::XMStoreFloat3(&up, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&up), rotMat));
		}

		const DirectX::XMFLOAT3 focusPoint = DirectX::XMFLOAT3(eye.x + lookDir.x, eye.y + lookDir.y, eye.z + lookDir.z);

		Camera::setView(DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&focusPoint), DirectX::XMLoadFloat3(&up)));
	}

	void render()
	{
		Renderer::clearDefRTV(DirectX::Colors::Black);
		Renderer::setDefRTV();
	}


};