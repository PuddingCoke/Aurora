#pragma once

#include<Aurora/EngineAPI/RenderPass.h>
#include<Aurora/EngineAPI/ResourceEssential.h>
#include<Aurora/EngineAPI/Graphics.h>
#include<Aurora/EngineAPI/Camera.h>

#include<Aurora/Effect/HBAOEffect.h>

#include"Scene.h"

class SceneRenderPass :public RenderPass
{
public:

	static constexpr UINT skyboxResolution = 1024;

	SceneRenderPass() :
		hbaoEffect(context, Graphics::getWidth(), Graphics::getHeight()),
		skybox(new TextureCube(assetPath + "/sky/kloppenheim_05_4k.hdr", skyboxResolution)),
		deferredVShader(new Shader(Utils::getRootFolder() + "DeferredVShader.cso", ShaderType::Vertex)),
		deferredPShader(new Shader(Utils::getRootFolder() + "DeferredPShader.cso", ShaderType::Pixel)),
		deferredFinal(new Shader(Utils::getRootFolder() + "DeferredFinal.cso", ShaderType::Pixel)),
		skyboxPShader(new Shader(Utils::getRootFolder() + "SkyboxPShader.cso", ShaderType::Pixel)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN, DirectX::Colors::Black))
	{
		//³õÊ¼»¯InputLayout
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
	}

	~SceneRenderPass()
	{
		delete shadowTexture;
		delete depthTexture;
		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete originTexture;
		delete irradianceBounceCoeff;
		delete depthOctahedralMap;
		delete skybox;
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete skyboxPShader;
		delete irradianceVolumeBuffer;
		delete lightBuffer;
		delete shadowProjBuffer;
		delete scene;
	}

	ComPtr<ID3D11InputLayout> modelInputLayout;

	ResourceDepthTexture* shadowTexture;

	ResourceDepthTexture* depthTexture;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	RenderTexture* originTexture;

	ComputeTexture* irradianceBounceCoeff;

	ComputeTexture* depthOctahedralMap;

	TextureCube* skybox;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	Shader* skyboxPShader;

	ConstantBuffer* irradianceVolumeBuffer;

	ConstantBuffer* lightBuffer;

	ConstantBuffer* shadowProjBuffer;

	Scene* scene;

private:

	void recordCommand() override
	{
		context->OMSetBlendState(nullptr);
		context->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->RSSetState(States::rasterCullBack);
		context->OMSetDepthStencilState(States::defDepthStencilState, 0);
		context->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		context->IASetInputLayout(modelInputLayout.Get());

		context->PSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);
		context->CSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);

		context->VSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
		context->HSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
		context->DSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
		context->GSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);

		context->ClearDSV(depthTexture, D3D11_CLEAR_DEPTH);

		context->ClearRTV(gBaseColor->getMip(0), DirectX::Colors::Transparent);
		context->ClearRTV(gPosition->getMip(0), DirectX::Colors::Transparent);
		context->ClearRTV(gNormalSpecular->getMip(0), DirectX::Colors::Transparent);

		context->OMSetRTV({ gPosition->getMip(0),gNormalSpecular->getMip(0),gBaseColor->getMip(0) }, depthTexture);
		context->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

		scene->render(context, deferredVShader, deferredPShader);

		context->OMSetDefRTV(nullptr);

		ShaderResourceView* hbaoSRV = hbaoEffect.process(depthTexture->getSRV(), gNormalSpecular->getSRV());

		context->ClearRTV(originTexture->getMip(0), DirectX::Colors::Black);
		context->OMSetRTV({ originTexture->getMip(0) }, nullptr);
		context->PSSetSRV({ gPosition,gNormalSpecular,gBaseColor,hbaoSRV,shadowTexture,irradianceBounceCoeff,depthOctahedralMap }, 0);
		context->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,shadowProjBuffer,irradianceVolumeBuffer }, 1);
		context->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

		context->BindShader(Shader::fullScreenVS);
		context->BindShader(deferredFinal);

		context->DrawQuad();

		context->OMSetRTV({ originTexture->getMip(0) }, depthTexture);

		context->PSSetSRV({ skybox }, 0);
		context->PSSetSampler({ States::linearClampSampler }, 0);

		context->BindShader(Shader::skyboxVS);
		context->BindShader(skyboxPShader);

		context->DrawCube();

		context->OMSetDefRTV(nullptr);

		context->OMSetBlendState(nullptr);

		context->OMSetDefRTV(nullptr);
	}

	HBAOEffect hbaoEffect;
};