#include<Aurora/A3D/CascadedShadowMap.h>

CascadedShadowMap::CascadedShadowMap(const DirectX::XMFLOAT3& lightDir) :
	lightProjBuffer(new Buffer(sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE))
{

}

CascadedShadowMap::~CascadedShadowMap()
{
	delete lightProjBuffer;
}

void CascadedShadowMap::renderShadowMap(std::function<void(void)> renderGeometry)
{
	DirectX::BoundingFrustum frustum;
	DirectX::BoundingFrustum::CreateFromMatrix(frustum, Camera::getProj());
	DirectX::XMMATRIX inverseView = DirectX::XMMatrixInverse(nullptr, Camera::getView());
	frustum.Transform(frustum, inverseView);

	DirectX::XMFLOAT3 corners[8];

	frustum.GetCorners(corners);

	DirectX::XMFLOAT3 origin = { 0,0,0 };

	for (const DirectX::XMFLOAT3& p : corners)
	{
		origin.x += p.x;
		origin.y += p.y;
		origin.z += p.z;
	}

	origin.x /= 8.f;
	origin.y /= 8.f;
	origin.z /= 8.f;

}
