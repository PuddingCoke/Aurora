#include<Aurora/Camera.h>

Camera* Camera::instance = nullptr;

const DirectX::XMMATRIX& Camera::getProj()
{
	return instance->projMatrix;
}

const DirectX::XMMATRIX& Camera::getView()
{
	return instance->viewMatrix;
}

Buffer* Camera::getProjBuffer()
{
	return instance->projBuffer;
}

Buffer* Camera::getViewBuffer()
{
	return instance->viewBuffer;
}

Camera::Camera() :
	fov(0), nearPlane(0), farPlane(0),
	projBuffer(new Buffer(sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
	viewBuffer(new Buffer(sizeof(ViewInfo), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE))
{
}

Camera::~Camera()
{
	delete projBuffer;
	delete viewBuffer;
}

void Camera::setProj(const DirectX::XMMATRIX& proj)
{
	instance->projMatrix = proj;
	const DirectX::XMMATRIX projTrans = DirectX::XMMatrixTranspose(proj);
	memcpy(instance->projBuffer->map(0).pData, &projTrans, sizeof(DirectX::XMMATRIX));
	instance->projBuffer->unmap(0);
}

void Camera::setProj(const float& fov, const float& aspectRatio, const float& zNear, const float& zFar)
{
	instance->fov = fov;
	instance->nearPlane = zNear;
	instance->farPlane = zFar;
	setProj(DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, zNear, zFar));
}

void Camera::setView(const DirectX::XMMATRIX& view)
{
	instance->viewMatrix = view;
	instance->viewInfo.view = DirectX::XMMatrixTranspose(instance->viewMatrix);

	memcpy(instance->viewBuffer->map(0).pData, &instance->viewInfo, sizeof(ViewInfo));
	instance->viewBuffer->unmap(0);
}

void Camera::setView(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	instance->viewInfo.eyePos = DirectX::XMFLOAT4(eye.x, eye.y, eye.z, 1.f);
	setView(DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&focus), DirectX::XMLoadFloat3(&up));
}

void Camera::setView(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up)
{
	DirectX::XMFLOAT3 eyePos;
	DirectX::XMStoreFloat3(&eyePos, eye);
	instance->viewInfo.eyePos = DirectX::XMFLOAT4(eyePos.x, eyePos.y, eyePos.z, 1.f);
	setView(DirectX::XMMatrixLookAtLH(eye, focus, up));
}

const float& Camera::getFov()
{
	return instance->fov;
}

const float& Camera::getNearPlane()
{
	return instance->nearPlane;
}

const float& Camera::getFarPlane()
{
	return instance->farPlane;
}
