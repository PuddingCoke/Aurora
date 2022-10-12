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

ID3D11Buffer* Camera::getProjBuffer()
{
	return instance->projBuffer.Get();
}

ID3D11Buffer* Camera::getViewBuffer()
{
	return instance->viewBuffer.Get();
}

Camera::Camera() :
	fov(0), nearPlane(0), farPlane(0)
{
	D3D11_BUFFER_DESC projDesc = {};
	projDesc.Usage = D3D11_USAGE_DYNAMIC;
	projDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
	projDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	projDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_BUFFER_DESC viewDesc = {};
	viewDesc.Usage = D3D11_USAGE_DYNAMIC;
	viewDesc.ByteWidth = sizeof(ViewInfo);
	viewDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	viewDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Renderer::device->CreateBuffer(&projDesc, nullptr, projBuffer.ReleaseAndGetAddressOf());
	Renderer::device->CreateBuffer(&viewDesc, nullptr, viewBuffer.ReleaseAndGetAddressOf());
}

void Camera::setProj(const DirectX::XMMATRIX& proj)
{
	instance->projMatrix = proj;
	const DirectX::XMMATRIX projTrans = DirectX::XMMatrixTranspose(proj);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(instance->projBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &projTrans, sizeof(DirectX::XMMATRIX));
	Renderer::context->Unmap(instance->projBuffer.Get(), 0);
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
	instance->viewInfo.normalMatrix = DirectX::XMMatrixInverse(nullptr, instance->viewMatrix);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(instance->viewBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &instance->viewInfo, sizeof(ViewInfo));
	Renderer::context->Unmap(instance->viewBuffer.Get(), 0);
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

DirectX::XMFLOAT3 Camera::toViewSpace(const DirectX::XMFLOAT3& pos)
{
	const DirectX::XMVECTOR transformed = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), instance->viewMatrix);
	DirectX::XMFLOAT3 outPosition;
	DirectX::XMStoreFloat3(&outPosition, transformed);
	return outPosition;
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
