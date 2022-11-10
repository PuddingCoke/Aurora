#pragma once

#ifndef _CASCADEDSHADOWMAP_H_
#define _CASCADEDSHADOWMAP_H_

#include<functional>

#include<Aurora/RenderAPI.h>
#include<Aurora/Camera.h>
#include<Aurora/Math.h>

#include"ShadowMap.h"

class CascadedShadowMap
{
public:

	CascadedShadowMap() = delete;

	CascadedShadowMap(const CascadedShadowMap&) = delete;

	void operator=(const CascadedShadowMap&) = delete;

	CascadedShadowMap(const DirectX::XMFLOAT3& lightDir);

	~CascadedShadowMap();

	void renderShadowMap(std::function<void(void)> renderGeometry);

private:

	static constexpr unsigned int shadowMapRes = 2048;

	static constexpr unsigned int shadowMapLevels = 4;

	Buffer* lightProjBuffer;

};

#endif // !_CASCADEDSHADOWMAP_H_