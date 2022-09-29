#include<Aurora/A3D/CascadedShadowMap.h>

CascadedShadowMap::CascadedShadowMap(const unsigned int& width, const unsigned int& height, const DirectX::XMFLOAT3& lightPos, const DirectX::XMFLOAT3& lightLookAt) :
	shadowCtx(nullptr)
{
	GFSDK_ShadowLib_Version ver;
	GFSDK_ShadowLib_GetDLLVersion(&ver);

	GFSDK_ShadowLib_DeviceContext deviceContext;

	deviceContext.pD3DDevice = Renderer::device.Get();
	deviceContext.pDeviceContext = Renderer::context.Get();

	std::cout << "[class CascadedShadowMap] NVIDIA ShadowLib create status " << GFSDK_ShadowLib_Create(&ver, &shadowCtx, &deviceContext) << "\n";

	//创建shadowBufferHandle shadowMapHandle
	{
		GFSDK_ShadowLib_BufferDesc sbDesc;
		sbDesc.uResolutionWidth = width;
		sbDesc.uResolutionHeight = height;
		sbDesc.uViewportLeft = 0;
		sbDesc.uViewportTop = 0;
		sbDesc.uViewportRight = width;
		sbDesc.uViewportBottom = height;
		sbDesc.uSampleCount = 1;

		GFSDK_ShadowLib_MapDesc smDesc;
		smDesc.eViewType = GFSDK_ShadowLib_ViewType_Cascades_4;
		smDesc.uResolutionWidth = shadowMapRes;
		smDesc.uResolutionHeight = shadowMapRes;

		smDesc.FrustumTraceMapDesc.bRequireFrustumTraceMap = true;
		smDesc.FrustumTraceMapDesc.uResolutionWidth = frustumTraceMapRes;
		smDesc.FrustumTraceMapDesc.uResolutionHeight = frustumTraceMapRes;
		smDesc.FrustumTraceMapDesc.uDynamicReprojectionCascades = 2;
		smDesc.FrustumTraceMapDesc.uQuantizedListLengthTexelDimension = 4;

		smDesc.RayTraceMapDesc.bRequirePrimitiveMap = true;
		smDesc.RayTraceMapDesc.uMaxNumberOfPrimitives = 1000000;
		smDesc.RayTraceMapDesc.uMaxNumberOfPrimitivesPerPixel = 64;
		smDesc.RayTraceMapDesc.uResolutionWidth = rayTraceMapRes;
		smDesc.RayTraceMapDesc.uResolutionHeight = rayTraceMapRes;

		shadowCtx->AddBuffer(&sbDesc, &shadowBufferHandle);
		shadowCtx->AddMap(&smDesc, &sbDesc, &shadowMapHandle);
	}

	//创建lightViewProjBuffer
	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(DirectX::XMMATRIX);
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Renderer::device->CreateBuffer(&bd, nullptr, lightViewProjBuffer.ReleaseAndGetAddressOf());
	}

	setLightPos(lightPos);
	setLightLookAt(lightLookAt);

	smRenderParams.LightDesc.eLightType = GFSDK_ShadowLib_LightType_Directional;
	smRenderParams.LightDesc.fLightSize = 2.0f;

	smRenderParams.ZBiasParams.iDepthBias = 0;
	smRenderParams.ZBiasParams.fSlopeScaledDepthBias = 0;
	smRenderParams.ZBiasParams.bUseReceiverPlaneBias = false;
	smRenderParams.ZBiasParams.fDistanceBiasMin = 0.000001f;
	smRenderParams.ZBiasParams.fDistanceBiasFactor = 0.0002f;
	smRenderParams.ZBiasParams.fDistanceBiasThreshold = 300.0f;
	smRenderParams.ZBiasParams.fDistanceBiasPower = 3.0f;
	smRenderParams.eCullModeType = GFSDK_ShadowLib_CullModeType_None;
	smRenderParams.eTechniqueType = GFSDK_ShadowLib_TechniqueType_HFTS;
	smRenderParams.PCSSPenumbraParams.fMinSizePercent[0] = 5.0f;
	smRenderParams.PCSSPenumbraParams.fMinSizePercent[1] = 5.0f;
	smRenderParams.PCSSPenumbraParams.fMinSizePercent[2] = 5.0f;
	smRenderParams.PCSSPenumbraParams.fMinSizePercent[3] = 5.0f;
	smRenderParams.PCSSPenumbraParams.fMinWeightThresholdPercent = 3.0f;
	smRenderParams.PCSSPenumbraParams.fMaxThreshold = 1000.0f;
	smRenderParams.eCascadedShadowMapType = GFSDK_ShadowLib_CascadedShadowMapType_SampleDistribution;

	smRenderParams.fCascadeMaxDistancePercent = 125.0f;
	smRenderParams.fCascadeZLinearScale[0] = 0.1f;
	smRenderParams.fCascadeZLinearScale[1] = 0.2f;
	smRenderParams.fCascadeZLinearScale[2] = 0.5f;
	smRenderParams.fCascadeZLinearScale[3] = 1.0f;

	smRenderParams.v3WorldSpaceBBox[0] = smRenderParams.v3WorldSpaceBBox[1] = GFSDK_Zero_Vector3;

	smRenderParams.FrustumTraceMapRenderParams.eConservativeRasterType = GFSDK_ShadowLib_ConservativeRasterType_HW;
	smRenderParams.FrustumTraceMapRenderParams.eCullModeType = GFSDK_ShadowLib_CullModeType_None;
	smRenderParams.FrustumTraceMapRenderParams.fHitEpsilon = 0.01f;

	smRenderParams.RayTraceMapRenderParams.eConservativeRasterType = GFSDK_ShadowLib_ConservativeRasterType_HW;
	smRenderParams.RayTraceMapRenderParams.eCullModeType = GFSDK_ShadowLib_CullModeType_None;
	smRenderParams.RayTraceMapRenderParams.fHitEpsilon = 0.06f;

}

CascadedShadowMap::~CascadedShadowMap()
{
	shadowCtx->Destroy();
}

DirectX::XMFLOAT3 CascadedShadowMap::getLightPos() const
{
	return { smRenderParams.LightDesc.v3LightPos[0].x,smRenderParams.LightDesc.v3LightPos[0].y, smRenderParams.LightDesc.v3LightPos[0].z };
}

DirectX::XMFLOAT3 CascadedShadowMap::getLightLookAt() const
{
	return { smRenderParams.LightDesc.v3LightLookAt[0].x,smRenderParams.LightDesc.v3LightLookAt[0].y, smRenderParams.LightDesc.v3LightLookAt[0].z };
}

void CascadedShadowMap::setLightPos(const DirectX::XMFLOAT3& lightPos)
{
	memcpy(&smRenderParams.LightDesc.v3LightPos[0], &lightPos, sizeof(gfsdk_float3));
	memcpy(&smRenderParams.LightDesc.v3LightPos[1], &lightPos, sizeof(gfsdk_float3));
	memcpy(&smRenderParams.LightDesc.v3LightPos[2], &lightPos, sizeof(gfsdk_float3));
	memcpy(&smRenderParams.LightDesc.v3LightPos[3], &lightPos, sizeof(gfsdk_float3));
}

void CascadedShadowMap::setLightLookAt(const DirectX::XMFLOAT3& lightLookAt)
{
	memcpy(&smRenderParams.LightDesc.v3LightLookAt[0], &lightLookAt, sizeof(gfsdk_float3));
	memcpy(&smRenderParams.LightDesc.v3LightLookAt[1], &lightLookAt, sizeof(gfsdk_float3));
	memcpy(&smRenderParams.LightDesc.v3LightLookAt[2], &lightLookAt, sizeof(gfsdk_float3));
	memcpy(&smRenderParams.LightDesc.v3LightLookAt[3], &lightLookAt, sizeof(gfsdk_float3));
}

void CascadedShadowMap::updateMatrices()
{
	const DirectX::XMMATRIX cameraProj = Camera::getProj();
	const DirectX::XMMATRIX cameraView = Camera::getView();
	memcpy(&smRenderParams.m4x4EyeProjectionMatrix, &cameraProj, sizeof(gfsdk_float4x4));
	memcpy(&smRenderParams.m4x4EyeViewMatrix, &cameraView, sizeof(gfsdk_float4x4));
}

void CascadedShadowMap::renderShaodwMap(ShadowMap* const shadowMap, std::function<void(void)> renderGeometry)
{
	smRenderParams.DepthBufferDesc.eDepthType = GFSDK_ShadowLib_DepthType_DepthBuffer;
	smRenderParams.DepthBufferDesc.DepthSRV.pSRV = shadowMap->getSRV();
	smRenderParams.DepthBufferDesc.ResolvedDepthSRV.pSRV = nullptr;
	smRenderParams.DepthBufferDesc.ReadOnlyDSV.pDSV = shadowMap->getROView();

	shadowCtx->SetMapRenderParams(shadowMapHandle, &smRenderParams);

	shadowCtx->UpdateMapBounds(shadowMapHandle, lightViewMatrices, lightProjMatrices, renderFrusta);

	Renderer::context->HSSetShader(nullptr, nullptr, 0);
	Renderer::context->DSSetShader(nullptr, nullptr, 0);
	Renderer::context->GSSetShader(nullptr, nullptr, 0);

	shadowCtx->InitializeMapRendering(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_Depth);

	for (unsigned int uView = 0; uView < 4; uView++)
	{
		shadowCtx->BeginMapRendering(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_Depth, uView);

		const DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&lightViewMatrices[uView]);
		const DirectX::XMMATRIX projMatrix = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&lightProjMatrices[uView]);
		const DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		Renderer::context->Map(lightViewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &viewProjMatrix, sizeof(DirectX::XMMATRIX));
		Renderer::context->Unmap(lightViewProjBuffer.Get(), 0);

		Renderer::context->VSSetConstantBuffers(2, 1, lightViewProjBuffer.GetAddressOf());

		renderGeometry();

		shadowCtx->EndMapRendering(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_Depth, uView);
	}
}

void CascadedShadowMap::beginRayTraceRender()
{
	shadowCtx->InitializeMapRendering(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_RT);

	shadowCtx->BeginMapRendering(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_RT, 0);

	const DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&lightViewMatrices[0]);
	const DirectX::XMMATRIX projMatrix = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&lightProjMatrices[0]);
	const DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(lightViewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &viewProjMatrix, sizeof(DirectX::XMMATRIX));
	Renderer::context->Unmap(lightViewProjBuffer.Get(), 0);

	Renderer::context->VSSetConstantBuffers(2, 1, lightViewProjBuffer.GetAddressOf());
}

void CascadedShadowMap::endRayTraceRender()
{
	shadowCtx->EndMapRendering(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_RT, 0);
}

void CascadedShadowMap::renderFrustumTrace(std::function<void(void)> renderGeometry)
{
	shadowCtx->InitializeMapRendering(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_FT);

	shadowCtx->BeginMapRendering(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_FT, 0);

	const DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&lightViewMatrices[0]);
	const DirectX::XMMATRIX projMatrix = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&lightProjMatrices[0]);
	const DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(lightViewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &viewProjMatrix, sizeof(DirectX::XMMATRIX));
	Renderer::context->Unmap(lightViewProjBuffer.Get(), 0);

	Renderer::context->VSSetConstantBuffers(2, 1, lightViewProjBuffer.GetAddressOf());

	renderGeometry();

	shadowCtx->EndMapRendering(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_FT, 0);
}

void CascadedShadowMap::incrementMapPrimitiveCounter(const unsigned int& primitiveCount)
{
	shadowCtx->IncrementMapPrimitiveCounter(shadowMapHandle, GFSDK_ShadowLib_MapRenderType_RT, primitiveCount);
}

ID3D11ShaderResourceView* CascadedShadowMap::getShadowBuffer()
{
	shadowCtx->ClearBuffer(shadowBufferHandle);

	shadowCtx->RenderBuffer(shadowMapHandle, shadowBufferHandle, &sbRenderParams);

	GFSDK_ShadowLib_ShaderResourceView shadowBufferSRV = {};

	shadowCtx->FinalizeBuffer(shadowBufferHandle, &shadowBufferSRV);

	return shadowBufferSRV.pSRV;
}
