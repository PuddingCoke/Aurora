#pragma once

#define __GFSDK_DX11__

#include<functional>

#include<Aurora/Renderer.h>
#include<Aurora/Camera.h>
#include<Aurora/Math.h>

#include"ShadowMap.h"

#include<GFSDK_ShadowLib/GFSDK_ShadowLib.h>

class CascadedShadowMap
{
public:

	CascadedShadowMap() = delete;

	CascadedShadowMap(const CascadedShadowMap&) = delete;

	void operator=(const CascadedShadowMap&) = delete;

	CascadedShadowMap(const unsigned int& width, const unsigned int& height, const DirectX::XMFLOAT3& lightPos, const DirectX::XMFLOAT3& lightLookAt = { 0,0,0 });

	~CascadedShadowMap();

	gfsdk_float4x4 lightProjMatrices[GFSDK_ShadowLib_ViewType_Cascades_4];
	gfsdk_float4x4 lightViewMatrices[GFSDK_ShadowLib_ViewType_Cascades_4];
	GFSDK_ShadowLib_Frustum renderFrustum[GFSDK_ShadowLib_ViewType_Cascades_4];

	DirectX::XMFLOAT3 getLightPos() const;

	DirectX::XMFLOAT3 getLightLookAt() const;

	void setLightPos(const DirectX::XMFLOAT3& lightPos);

	void setLightLookAt(const DirectX::XMFLOAT3& lightLookAt);

	void updateCSMCamera();

	void renderShaodwMap(ShadowMap* const shadowMap, std::function<void(void)> renderFunc);

private:

	static constexpr unsigned int shadowMapRes = 2048;

	static constexpr unsigned int frustumTraceMapRes = 2048;

	static constexpr unsigned int rayTraceMapRes = 2048;

	GFSDK_ShadowLib_Context* shadowCtx;

	GFSDK_ShadowLib_Buffer* shadowBufferHandle;

	GFSDK_ShadowLib_Map* shadowMapHandle;

	GFSDK_ShadowLib_BufferRenderParams sbRenderParams;

	GFSDK_ShadowLib_MapRenderParams smRenderParams;

	ComPtr<ID3D11Buffer> lightViewProjBuffer;

};