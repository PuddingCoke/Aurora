#pragma once

#include<Aurora/Game.h>

#include<Aurora/Camera/FPSCamera.h>

#include"LightProbeUpdatePass.h"
#include"SceneRenderPass.h"
#include"PostProcessPass.h"

#include"Scene.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	FPSCamera camera;

	static constexpr UINT shadowMapRes = 4096;

	struct IrradianceVolumeParam
	{
		DirectX::XMFLOAT3 start = { -142.f,-16.f,-74.f };
		float spacing = 18.2f;
		DirectX::XMUINT3 count = { 17,9,12 };
		float irradianceDistanceBias = 0.8f;
		float irradianceVarianceBias = 0.f;
		float irradianceChebyshevBias = -1.0f;
		DirectX::XMFLOAT2 padding;
	} irradianceVolumeParam;

	struct Light
	{
		DirectX::XMVECTOR lightDir;
		DirectX::XMVECTOR lightColor;
	} light{};

	LightProbeUpdatePass* lightProbeUpdatePass;

	SceneRenderPass* sceneRenderPass;

	PostProcessPass* postProcessPass;

	MyGame() :
		camera({ 0.f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f),
		lightProbeUpdatePass(new LightProbeUpdatePass()),
		sceneRenderPass(new SceneRenderPass()),
		postProcessPass(new PostProcessPass()),
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA32F, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA32F, DirectX::Colors::Black)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F, DirectX::Colors::Black)),
		depthTexture(new ResourceDepthTexture(Graphics::getWidth(), Graphics::getHeight())),
		shadowTexture(new ResourceDepthTexture(shadowMapRes, shadowMapRes)),
		lightBuffer(new ConstantBuffer(sizeof(Light), D3D11_USAGE_DYNAMIC)),
		shadowProjBuffer(new ConstantBuffer(sizeof(DirectX::XMMATRIX), D3D11_USAGE_DYNAMIC)),
		scene(Scene::create(assetPath + "/sponza.dae")),
		sunAngle(Math::half_pi - 0.01f)
	{
		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);

		Shader* deferredVShader = new Shader(Utils::getRootFolder() + "DeferredVShader.cso", ShaderType::Vertex);

		//初始化InputLayout
		{
			D3D11_INPUT_ELEMENT_DESC layout[5] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			GraphicsDevice::get()->createInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(deferredVShader), modelInputLayout.ReleaseAndGetAddressOf());
		}

		delete deferredVShader;

		irradianceBounceCoeff = new ComputeTexture(9, 1, FMT::RG11B10F, FMT::RG11B10F, FMT::RG11B10F, 1, irradianceVolumeParam.count.x * irradianceVolumeParam.count.y * irradianceVolumeParam.count.z);
		depthOctahedralMap = new ComputeTexture(16, 16, FMT::RG16F, FMT::RG16F, FMT::RG16F, 1, irradianceVolumeParam.count.x * irradianceVolumeParam.count.y * irradianceVolumeParam.count.z);

		irradianceVolumeBuffer = new ConstantBuffer(sizeof(IrradianceVolumeParam), D3D11_USAGE_DYNAMIC, &irradianceVolumeParam);

		{
			lightProbeUpdatePass->modelInputLayout = modelInputLayout;
			lightProbeUpdatePass->scene = new Scene(*scene);
			lightProbeUpdatePass->shadowTexture = new ResourceDepthTexture(*shadowTexture);
			lightProbeUpdatePass->irradianceBounceCoeff = new ComputeTexture(*irradianceBounceCoeff);
			lightProbeUpdatePass->depthOctahedralMap = new ComputeTexture(*depthOctahedralMap);
			lightProbeUpdatePass->irradianceVolumeBuffer = irradianceVolumeBuffer;
			lightProbeUpdatePass->lightBuffer = lightBuffer;
			lightProbeUpdatePass->shadowProjBuffer = shadowProjBuffer;
		}

		{
			sceneRenderPass->modelInputLayout = modelInputLayout;
			sceneRenderPass->scene = new Scene(*scene);
			sceneRenderPass->shadowTexture = new ResourceDepthTexture(*shadowTexture);
			sceneRenderPass->depthTexture = new ResourceDepthTexture(*depthTexture);
			sceneRenderPass->gPosition = new RenderTexture(*gPosition);
			sceneRenderPass->gNormalSpecular = new RenderTexture(*gNormalSpecular);
			sceneRenderPass->originTexture = new RenderTexture(*originTexture);
			sceneRenderPass->irradianceBounceCoeff = new ComputeTexture(*irradianceBounceCoeff);
			sceneRenderPass->depthOctahedralMap = new ComputeTexture(*depthOctahedralMap);
			sceneRenderPass->irradianceVolumeBuffer = irradianceVolumeBuffer;
			sceneRenderPass->lightBuffer = lightBuffer;
			sceneRenderPass->shadowProjBuffer = shadowProjBuffer;
		}

		{
			postProcessPass->depthTexture = new ResourceDepthTexture(*depthTexture);
			postProcessPass->gPosition = new RenderTexture(*gPosition);
			postProcessPass->gNormalSpecular = new RenderTexture(*gNormalSpecular);
			postProcessPass->originTexture = new RenderTexture(*originTexture);
		}

		updateShadow();
		auto pass0 = lightProbeUpdatePass->GetPassResult();
		auto cmd0 = pass0.get();
		ImCtx::get()->ExecuteCommandList(cmd0);
		cmd0->Release();
	}

	~MyGame()
	{
		delete lightProbeUpdatePass;
		delete sceneRenderPass;
		delete postProcessPass;

		delete shadowTexture;
		delete depthTexture;

		delete gPosition;
		delete gNormalSpecular;
		delete originTexture;

		delete irradianceBounceCoeff;
		delete depthOctahedralMap;

		delete irradianceVolumeBuffer;
		delete lightBuffer;
		delete shadowProjBuffer;

		delete scene;
	}

	void imGUICall() override
	{
		ImGui::SliderFloat("irradiance distance bias", &irradianceVolumeParam.irradianceDistanceBias, -5.f, 5.f);
		ImGui::SliderFloat("irradiance variance bias", &irradianceVolumeParam.irradianceVarianceBias, -5.f, 5.f);
		ImGui::SliderFloat("irradiance chebyshev bias", &irradianceVolumeParam.irradianceChebyshevBias, -5.f, 5.f);
		postProcessPass->bloomEffect.imGUIEffectModifier();
		postProcessPass->fxaaEffect.imGUIEffectModifier();
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);

		if (Keyboard::getKeyDown(Keyboard::Q))
		{
			sunAngle += dt;
			sunAngle = Math::clamp(sunAngle, Math::half_pi - 0.365f, Math::half_pi + 0.365f);
			updateShadow();
			auto pass0 = lightProbeUpdatePass->GetPassResult();
			auto cmd0 = pass0.get();
			ImCtx::get()->ExecuteCommandList(cmd0);
			cmd0->Release();
		}
		else if (Keyboard::getKeyDown(Keyboard::E))
		{
			sunAngle -= dt;
			sunAngle = Math::clamp(sunAngle, Math::half_pi - 0.365f, Math::half_pi + 0.365f);
			updateShadow();
			auto pass0 = lightProbeUpdatePass->GetPassResult();
			auto cmd0 = pass0.get();
			ImCtx::get()->ExecuteCommandList(cmd0);
			cmd0->Release();
		}
	}

	void render()
	{
		auto pass0 = sceneRenderPass->GetPassResult();

		auto pass1 = postProcessPass->GetPassResult();

		CommandListArray::pushCommandList(pass0.get());
		CommandListArray::pushCommandList(pass1.get());
	}

private:

	//光照方向改变
	void updateShadow()
	{
		const float xSize = 183;
		const float ySize = 130;
		const float distance = 260.f;
		const DirectX::XMVECTOR offset = { 6.5f,0.f,0.f };

		light.lightDir = { 0.f,sinf(sunAngle),cosf(sunAngle),0.f };
		light.lightColor = DirectX::Colors::White;

		light.lightDir = DirectX::XMVector3Normalize(light.lightDir);

		const DirectX::XMVECTOR lightCamPos = DirectX::XMVectorAdd(DirectX::XMVectorScale(light.lightDir, distance), offset);
		const DirectX::XMMATRIX lightProjMat = DirectX::XMMatrixOrthographicLH(xSize, ySize, 1.f, 512.f);
		const DirectX::XMMATRIX lightViewMat = DirectX::XMMatrixLookAtLH(lightCamPos, offset, { 0.f,1.f,0.f });
		const DirectX::XMMATRIX lightMat = DirectX::XMMatrixTranspose(lightViewMat * lightProjMat);

		memcpy(ImCtx::get()->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &light, sizeof(Light));
		ImCtx::get()->Unmap(lightBuffer, 0);

		memcpy(ImCtx::get()->Map(shadowProjBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &lightMat, sizeof(DirectX::XMMATRIX));
		ImCtx::get()->Unmap(shadowProjBuffer, 0);

		ImCtx::get()->OMSetBlendState(nullptr);
		ImCtx::get()->IASetInputLayout(modelInputLayout.Get());

		ImCtx::get()->OMSetDepthStencilState(States::defDepthStencilState, 0);
		ImCtx::get()->RSSetState(States::rasterShadow);
		ImCtx::get()->RSSetViewport(shadowMapRes, shadowMapRes);

		ImCtx::get()->ClearDSV(shadowTexture, D3D11_CLEAR_DEPTH);

		ImCtx::get()->OMSetRTV({}, shadowTexture);
		ImCtx::get()->VSSetConstantBuffer({ shadowProjBuffer }, 2);

		scene->renderGeometry(ImCtx::get(), Shader::shadowVS);

		ImCtx::get()->RSSetState(States::rasterCullBack);
	}

	ComPtr<ID3D11InputLayout> modelInputLayout;

	ResourceDepthTexture* shadowTexture;

	ResourceDepthTexture* depthTexture;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* originTexture;

	ComputeTexture* irradianceBounceCoeff;

	ComputeTexture* depthOctahedralMap;

	ConstantBuffer* irradianceVolumeBuffer;

	ConstantBuffer* lightBuffer;

	ConstantBuffer* shadowProjBuffer;

	Scene* scene;

	float sunAngle;


};