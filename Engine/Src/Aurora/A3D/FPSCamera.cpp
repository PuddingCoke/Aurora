#include<Aurora/A3D/FPSCamera.h>

FPSCamera::FPSCamera(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& lookDir, const DirectX::XMFLOAT3& up, const float& moveSpeed, const float& rotationSpeed) :
	eye(eye), lookDir(lookDir), up(up), moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
{
	DirectX::XMFLOAT3 lookDirNorm;

	DirectX::XMStoreFloat3(&lookDirNorm, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&lookDir)));

	const DirectX::XMFLOAT3 focusPoint = DirectX::XMFLOAT3(eye.x + lookDirNorm.x, eye.y + lookDirNorm.y, eye.z + lookDirNorm.z);

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

				float lookUpAngle;

				DirectX::XMStoreFloat(&lookUpAngle, DirectX::XMVector3AngleBetweenNormals(DirectX::XMLoadFloat3(&lookDir), DirectX::XMLoadFloat3(&up)));

				const float destAngle = lookUpAngle - Mouse::getDY() / 120.f;

				float rotAngle = -Mouse::getDY() / 120.f;

				//做一下俯仰角限制
				if (destAngle > Math::pi - Camera::epsilon)
				{
					rotAngle = Math::pi - Camera::epsilon - lookUpAngle;
				}
				else if (destAngle < Camera::epsilon)
				{
					rotAngle = Camera::epsilon - lookUpAngle;
				}

				const DirectX::XMMATRIX lookDirRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, rotAngle);

				DirectX::XMStoreFloat3(&lookDir, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&lookDir), lookDirRotMat));
			}
		});
}

const DirectX::XMFLOAT3& FPSCamera::getEye() const
{
	return eye;
}
