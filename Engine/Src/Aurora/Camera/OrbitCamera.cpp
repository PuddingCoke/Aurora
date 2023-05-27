#include<Aurora/Camera/OrbitCamera.h>

OrbitCamera::OrbitCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& up, const float& scaleSpeed) :
	eye(DirectX::XMVector3Normalize(eye)), up(DirectX::XMVector3Normalize(up)), scaleSpeed(scaleSpeed)
{
	DirectX::XMStoreFloat(&length, DirectX::XMVector3Length(eye));
	curLength = length;
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

				DirectX::XMStoreFloat(&eyeUpAngle, DirectX::XMVector3AngleBetweenNormals(eye, up));

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
			}
		});

	Mouse::addScrollEvent([this]()
		{
			length -= 0.5f * scaleSpeed * Mouse::getWheelDelta();
	if (length < 0.1f)
	{
		length = 0.1f;
	}
		});
}

void OrbitCamera::applyInput(const float& dt)
{
	curLength = Math::lerp(curLength, length, Math::clamp(dt * 20.f, 0.f, 1.f));

	Camera::setView(DirectX::XMVectorScale(eye, curLength), { 0,0,0 }, up);
}

void OrbitCamera::rotateX(const float& angle)
{
	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(up, angle);

	eye = DirectX::XMVector3Transform(eye, rotMat);
}