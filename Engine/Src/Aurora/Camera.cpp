#include<Aurora/Camera.h>

DirectX::XMFLOAT3 Camera::eye = { 0.f,20.f,0.f };

DirectX::XMFLOAT3 Camera::lookDir = { 1.f,0.f,0.f };

DirectX::XMFLOAT3 Camera::up = { 0.f,1.f,0.f };

ComPtr<ID3D11Buffer> Camera::projBuffer;

ComPtr<ID3D11Buffer> Camera::viewBuffer;

DirectX::XMMATRIX Camera::viewMatrix;

Camera::ViewMatrices Camera::viewMatrices;

void Camera::setProj(const DirectX::XMMATRIX& proj)
{
	const DirectX::XMMATRIX projTrans = DirectX::XMMatrixTranspose(proj);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Graphics::context->Map(projBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &projTrans, sizeof(DirectX::XMMATRIX));
	Graphics::context->Unmap(projBuffer.Get(), 0);
}

void Camera::setView(const DirectX::XMMATRIX& view)
{
	viewMatrix = view;
	viewMatrices.view = DirectX::XMMatrixTranspose(viewMatrix);
	viewMatrices.normalMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Graphics::context->Map(viewBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &viewMatrices, sizeof(ViewMatrices));
	Graphics::context->Unmap(viewBuffer.Get(), 0);
}

DirectX::XMFLOAT3 Camera::toViewSpace(const DirectX::XMFLOAT3& pos)
{
	const DirectX::XMVECTOR transformed = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), viewMatrix);
	DirectX::XMFLOAT3 outPosition;
	DirectX::XMStoreFloat3(&outPosition, transformed);
	return outPosition;
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
	viewDesc.ByteWidth = sizeof(ViewMatrices);
	viewDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	viewDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Graphics::device->CreateBuffer(&projDesc, nullptr, projBuffer.ReleaseAndGetAddressOf());
	Graphics::device->CreateBuffer(&viewDesc, nullptr, viewBuffer.ReleaseAndGetAddressOf());
}
