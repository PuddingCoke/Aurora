#pragma once

#include<Aurora/Game.h>

#include<Aurora/Camera/FPSCamera.h>

#include"OceanRenderPass.h"
#include"PostProcessPass.h"

class MyGame :public Game
{
public:

	FPSCamera camera;

	RenderTexture* originTexture;

	OceanRenderPass* oceanRenderPass;

	PostProcessPass* postProcessPass;

	MyGame() :
		camera({ 512,100,512 }, { 0,-0.2f,-1.f }, { 0,1,0 }, 100),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F)),
		oceanRenderPass(new OceanRenderPass(originTexture)),
		postProcessPass(new PostProcessPass(originTexture))
	{
		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 4096.f);
	}

	~MyGame()
	{
		delete originTexture;
		delete oceanRenderPass;
		delete postProcessPass;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void imGUICall() override
	{

	}

	void render() override
	{
		auto pass0 = oceanRenderPass->GetPassResult();

		auto pass1 = postProcessPass->GetPassResult();

		CommandListArray::pushCommandList(pass0.get());

		CommandListArray::pushCommandList(pass1.get());
	}


};