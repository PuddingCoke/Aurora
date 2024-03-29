#include<Aurora/EngineAPI/Camera.h>

Camera* Camera::instance = nullptr;

const DirectX::XMMATRIX& Camera::getProj()
{
	return instance->projMatrix;
}

const DirectX::XMMATRIX& Camera::getView()
{
	return instance->viewMatrix;
}

ConstantBuffer* Camera::getProjBuffer()
{
	return instance->projBuffer;
}

ConstantBuffer* Camera::getViewBuffer()
{
	return instance->viewBuffer;
}

const DirectX::XMVECTOR& Camera::getEye()
{
	return instance->viewInfo.eyePos;
}

Camera::Camera() :
	projBuffer(new ConstantBuffer(sizeof(DirectX::XMMATRIX), D3D11_USAGE_DEFAULT)),
	viewBuffer(new ConstantBuffer(sizeof(ViewInfo), D3D11_USAGE_DYNAMIC))
{
	instance = this;
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
	ImCtx::get()->UpdateSubresource(instance->projBuffer, 0, nullptr, &projTrans, 0, 0);
}

void Camera::setProj(const float& fov, const float& aspectRatio, const float& zNear, const float& zFar)
{
	setProj(DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, zNear, zFar));
}

void Camera::setView(const DirectX::XMMATRIX& view)
{
	instance->viewInfo.prevViewProj = DirectX::XMMatrixTranspose(instance->viewMatrix * instance->projMatrix);

	instance->viewMatrix = view;
	instance->viewInfo.view = DirectX::XMMatrixTranspose(instance->viewMatrix);
	instance->viewInfo.viewProj = DirectX::XMMatrixTranspose(instance->viewMatrix * instance->projMatrix);
	instance->viewInfo.normalMatrix = DirectX::XMMatrixInverse(nullptr, instance->viewMatrix);
}

void Camera::updateViewBuffer()
{
	BufferUpdate::pushBufferUpdateParam(viewBuffer, &viewInfo, sizeof(ViewInfo));
}

void Camera::setView(const DirectX::XMVECTOR& eye, const DirectX::XMVECTOR& focus, const DirectX::XMVECTOR& up)
{
	instance->viewInfo.eyePos = eye;
	setView(DirectX::XMMatrixLookAtLH(eye, focus, up));
}
