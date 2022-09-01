#include<Aurora/A3D/OrbitCamera.h>

OrbitCamera::OrbitCamera(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& up) :
	eye(eye), up(up), deltaAngle(0)
{
	DirectX::XMStoreFloat(&originAngle, DirectX::XMVector3AngleBetweenVectors(DirectX::XMLoadFloat3(&up), DirectX::XMLoadFloat3(&eye)));

	Camera::setView(eye, focus, up);
}

void OrbitCamera::registerEvent()
{
	Mouse::addMoveEvent([this]()
		{
			if (Mouse::getLeftDown())
			{
				const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&up), Mouse::getDX() / 120.f);

				DirectX::XMStoreFloat3(&eye, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&eye), rotMat));

				const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&up), DirectX::XMLoadFloat3(&eye));

				deltaAngle += Mouse::getDY() / 120.f;

				//限制一下俯仰角
				if (deltaAngle + originAngle < 0.001f)
				{
					deltaAngle = 0.001f - originAngle;
				}
				else if (deltaAngle + originAngle > Math::pi - 0.001f)
				{
					deltaAngle = Math::pi - 0.001f - originAngle;
				}

				const DirectX::XMMATRIX upRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, deltaAngle);

				DirectX::XMVECTOR rotatedEye = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&eye), upRotMat);

				Camera::setView(DirectX::XMMatrixLookAtLH(rotatedEye, DirectX::XMLoadFloat3(&focus), DirectX::XMLoadFloat3(&up)));
			}
		});

	Mouse::addScrollEvent([this]()
		{
			DirectX::XMStoreFloat3(&eye, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&eye), DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&eye)), -2.f * Mouse::getWheelDelta())));

			const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&up), DirectX::XMLoadFloat3(&eye));

			const DirectX::XMMATRIX upRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, deltaAngle);

			DirectX::XMVECTOR rotatedEye = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&eye), upRotMat);

			Camera::setView(DirectX::XMMatrixLookAtLH(rotatedEye, DirectX::XMLoadFloat3(&focus), DirectX::XMLoadFloat3(&up)));
		});
}

void OrbitCamera::rotateX(const float& angle)
{
	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&up), angle);

	DirectX::XMStoreFloat3(&eye, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&eye), rotMat));

	Camera::setView(eye, focus, up);
}