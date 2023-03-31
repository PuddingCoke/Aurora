#pragma once

#include<Aurora/Game.h>
#include<Aurora/A3D/FPSCamera.h>

#include"GlobalIllumination.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	FPSCamera camera;

	GlobalIllumination globalIllumination;

	MyGame() :
		camera({ 0.f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f)
	{
		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);
	}

	~MyGame()
	{
	}

	void imGUICall() override
	{
		
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);

		globalIllumination.update(dt);
	}

	void render()
	{
		/*gBaseColor->clearRTV(DirectX::Colors::Black);
		gPosition->clearRTV(DirectX::Colors::Black);
		gNormalSpecular->clearRTV(DirectX::Colors::Black);

		RenderAPI::get()->OMSetRTV({ gPosition,gNormalSpecular,gBaseColor }, depthTexture);

		scene->render(deferredVShader, deferredPShader);

		originTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);
		RenderAPI::get()->PSSetSRV({ gPosition,gNormalSpecular,gBaseColor,hbaoEffect.process(depthTexture->getSRV(), gNormalSpecular->getSRV()),globalShadowTexture}, 0);
		RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,lightProjBuffer }, 1);

		RenderAPI::fullScreenVS->use();
		deferredFinal->use();

		RenderAPI::get()->DrawQuad();

		RenderAPI::get()->OMSetRTV({ originTexture }, depthTexture);
		
		RenderAPI::get()->PSSetSRV({ skybox }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		RenderAPI::skyboxVS->use();
		skyboxPShader->use();

		RenderAPI::get()->DrawCube();

		ShaderResourceView* const bloomTextureSRV = bloomEffect.process(originTexture);

		RenderAPI::get()->OMSetBlendState(nullptr);

		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->PSSetSRV({ bloomTextureSRV }, 0);
		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();
		RenderAPI::get()->DrawQuad();*/

		/*if (showRadiance)
		{
			RenderAPI::get()->PSSetSRV({ distanceCube }, 0);

			RenderAPI::skyboxVS->use();
			skyboxPShader->use();

			RenderAPI::get()->DrawCube();
		}
		else
		{
			RenderAPI::get()->PSSetSRV({ depthOctahedralMap }, 0);

			RenderAPI::skyboxVS->use();
			octahedralDecode->use();

			RenderAPI::get()->DrawCube();
		}*/

		/*if (showRadiance)
		{
			RenderAPI::get()->PSSetSRV({ radianceCube }, 0);

			RenderAPI::skyboxVS->use();
			skyboxPShader->use();

			RenderAPI::get()->DrawCube();
		}
		else
		{
			RenderAPI::get()->PSSetSRV({ irradianceCoeff }, 0);

			RenderAPI::skyboxVS->use();
			irradianceEvaluate->use();

			RenderAPI::get()->DrawCube();
		}*/

	}


};