#include<Aurora/A3D/FPSCamera.h>

FPSCamera::FPSCamera(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& lookDir, const DirectX::XMFLOAT3& up, const float& moveSpeed, const float& rotationSpeed) :
	eye(eye), lookDir(lookDir), up(up), moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
{
	const DirectX::XMFLOAT3 focusPoint = DirectX::XMFLOAT3(eye.x + lookDir.x, eye.y + lookDir.y, eye.z + lookDir.z);

	Camera::setView(eye, focusPoint, up);
}

void FPSCamera::applyInput(const float& dt)
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

	Camera::setView(eye, focusPoint, up);
}

void FPSCamera::registerEvent()
{
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

const DirectX::XMFLOAT3& FPSCamera::getEye() const
{
	return eye;
}
