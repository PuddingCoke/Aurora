#include<Aurora/Camera.h>

ComPtr<ID3D11Buffer> Camera::projBuffer;

ComPtr<ID3D11Buffer> Camera::viewBuffer;

DirectX::XMMATRIX Camera::projMatrix;

DirectX::XMMATRIX Camera::viewMatrix;

Camera::ViewInfo Camera::viewInfo;

float Camera::fov = 0.f;

float Camera::nearPlane = 0.f;

float Camera::farPlane = 0.f;

const DirectX::XMMATRIX& Camera::getProj()
{
	return projMatrix;
}

const DirectX::XMMATRIX& Camera::getView()
{
	return viewMatrix;
}

ID3D11Buffer* Camera::getProjBuffer()
{
	return projBuffer.Get();
}

ID3D11Buffer* Camera::getViewBuffer()
{
	return viewBuffer.Get();
}

void Camera::setProj(const DirectX::XMMATRIX& proj)
{
	projMatrix = proj;
	const DirectX::XMMATRIX projTrans = DirectX::XMMatrixTranspose(proj);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(projBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &projTrans, sizeof(DirectX::XMMATRIX));
	Renderer::context->Unmap(projBuffer.Get(), 0);
}

void Camera::setProj(const float& fov, const float& aspectRatio, const float& zNear, const float& zFar)
{
	Camera::fov = fov;
	Camera::nearPlane = zNear;
	Camera::farPlane = zFar;
	setProj(DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, zNear, zFar));
}

void Camera::setView(const DirectX::XMMATRIX& view)
{
	viewMatrix = view;
	viewInfo.view = DirectX::XMMatrixTranspose(viewMatrix);
	viewInfo.normalMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(viewBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &viewInfo, sizeof(ViewInfo));
	Renderer::context->Unmap(viewBuffer.Get(), 0);
}

void Camera::setView(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	viewInfo.eyePos = DirectX::XMFLOAT4(eye.x, eye.y, eye.z, 1.f);
	setView(DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&focus), DirectX::XMLoadFloat3(&up));
}

void Camera::setView(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up)
{
	DirectX::XMFLOAT3 eyePos;
	DirectX::XMStoreFloat3(&eyePos, eye);
	viewInfo.eyePos = DirectX::XMFLOAT4(eyePos.x, eyePos.y, eyePos.z, 1.f);
	setView(DirectX::XMMatrixLookAtLH(eye, focus, up));
}

DirectX::XMFLOAT3 Camera::toViewSpace(const DirectX::XMFLOAT3& pos)
{
	const DirectX::XMVECTOR transformed = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), viewMatrix);
	DirectX::XMFLOAT3 outPosition;
	DirectX::XMStoreFloat3(&outPosition, transformed);
	return outPosition;
}

const float& Camera::getFov()
{
	return fov;
}

const float& Camera::getNearPlane()
{
	return nearPlane;
}

const float& Camera::getFarPlane()
{
	return farPlane;
}

void Camera::initialize()
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
