#include<Aurora/A3D/OrbitCamera.h>

OrbitCamera::OrbitCamera(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& up) :
	eye(DirectX::XMLoadFloat3(&eye)), up(DirectX::XMLoadFloat3(&up))
{
	const DirectX::XMFLOAT3 focusP = { 0,0,0 };

	DirectX::XMLoadFloat3(&focusP);

	Camera::setView(this->eye, focus, this->up);
}

void OrbitCamera::registerEvent()
{
	Mouse::addMoveEvent([this]()
		{
			if (Mouse::getLeftDown())
			{
				const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(up, Mouse::getDX() / 120.f);

				eye = DirectX::XMVector3Transform(eye, rotMat);

				float eyeUpAngle;

				DirectX::XMStoreFloat(&eyeUpAngle, DirectX::XMVector3AngleBetweenVectors(eye, up));

				const float destAngle = eyeUpAngle + Mouse::getDY() / 120.f;

				float rotAngle = Mouse::getDY() / 120.f;

				if (destAngle > Math::pi - Camera::epsilon)
				{
					rotAngle = Math::pi - Camera::epsilon - eyeUpAngle;
				}
				else if (destAngle < Camera::epsilon)
				{
					rotAngle = Camera::epsilon - eyeUpAngle;
				}

				const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(up, eye);

				const DirectX::XMMATRIX upRotMat = DirectX::XMMatrixRotationAxis(upCrossLookDir, rotAngle);

				eye = DirectX::XMVector3Transform(eye, upRotMat);

				Camera::setView(eye, focus, up);
			}
		});

	Mouse::addScrollEvent([this]()
		{
			eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(DirectX::XMVector3Normalize(eye), -2.f * Mouse::getWheelDelta()));

			Camera::setView(eye, focus, up);
		});
}

void OrbitCamera::rotateX(const float& angle)
{
	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(up, angle);

	eye = DirectX::XMVector3Transform(eye, rotMat);

	Camera::setView(eye, focus, up);
}