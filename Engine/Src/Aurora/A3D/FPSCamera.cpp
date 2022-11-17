#include<Aurora/A3D/FPSCamera.h>

FPSCamera::FPSCamera(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& lookDir, const DirectX::XMVECTOR& up, const float& moveSpeed) :
	eye(eye), lookDir(lookDir), up(up), moveSpeed(moveSpeed)
{
	this->lookDir = DirectX::XMVector3Normalize(this->lookDir);
	Camera::setView(this->eye, DirectX::XMVectorAdd(this->eye, this->lookDir), this->up);
}

void FPSCamera::applyInput(const float& dt)
{
	if (Keyboard::getKeyDown(Keyboard::W))
	{
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(lookDir, dt * moveSpeed));
	}

	if (Keyboard::getKeyDown(Keyboard::S))
	{
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(lookDir, -dt * moveSpeed));
	}

	if (Keyboard::getKeyDown(Keyboard::A))
	{
		const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(up, lookDir);
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(upCrossLookDir, -dt * moveSpeed));
	}

	if (Keyboard::getKeyDown(Keyboard::D))
	{
		const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(up, lookDir);
		eye = DirectX::XMVectorAdd(eye, DirectX::XMVectorScale(upCrossLookDir, dt * moveSpeed));
	}

	Camera::setView(eye, DirectX::XMVectorAdd(eye, lookDir), up);
}

void FPSCamera::registerEvent()
{
	Mouse::addMoveEvent([this]()
		{
			if (Mouse::getLeftDown())
			{
				const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationAxis(up, Mouse::getDX() / 120.f);

				lookDir = DirectX::XMVector3Transform(lookDir, rotMat);

				const DirectX::XMVECTOR upCrossLookDir = DirectX::XMVector3Cross(up, lookDir);

				float lookUpAngle;

				DirectX::XMStoreFloat(&lookUpAngle, DirectX::XMVector3AngleBetweenNormals(lookDir, up));

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

				lookDir = DirectX::XMVector3Transform(lookDir, lookDirRotMat);
			}
		});
}
