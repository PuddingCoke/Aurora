#pragma once

#ifndef _CASCADEDSHADOWMAP_H_
#define _CASCADEDSHADOWMAP_H_

#define __GFSDK_DX11__

#include<functional>

#include<Aurora/RenderAPI.h>
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

	DirectX::XMFLOAT3 getLightPos() const;

	DirectX::XMFLOAT3 getLightLookAt() const;

	void setLightPos(const DirectX::XMFLOAT3& lightPos);

	void setLightLookAt(const DirectX::XMFLOAT3& lightLookAt);

	void updateMatrices();

	void renderShaodwMap(ShadowMap* const shadowMap, std::function<void(void)> renderGeometry);

	void renderFrustumTrace(std::function<void(void)> renderGeometry);

	void beginRayTraceRender();

	void endRayTraceRender();

	void incrementMapPrimitiveCounter(const unsigned int& primitiveCount);

	ID3D11ShaderResourceView* getShadowBuffer();

private:

	static constexpr unsigned int shadowMapRes = 2048;

	static constexpr unsigned int frustumTraceMapRes = 2048;

	static constexpr unsigned int rayTraceMapRes = 2048;

	gfsdk_float4x4 lightProjMatrices[GFSDK_ShadowLib_ViewType_Cascades_4];

	gfsdk_float4x4 lightViewMatrices[GFSDK_ShadowLib_ViewType_Cascades_4];

	GFSDK_ShadowLib_Frustum renderFrusta[GFSDK_ShadowLib_ViewType_Cascades_4];

	GFSDK_ShadowLib_Context* shadowCtx;

	GFSDK_ShadowLib_Buffer* shadowBufferHandle;

	GFSDK_ShadowLib_Map* shadowMapHandle;

	GFSDK_ShadowLib_BufferRenderParams sbRenderParams;

	GFSDK_ShadowLib_MapRenderParams smRenderParams;

	Buffer* lightViewProjBuffer;

};

#endif // !_CASCADEDSHADOWMAP_H_