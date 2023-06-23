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
		camera({ 1024,100,3584 }, { 0,-0.2f,-1.f }, { 0,1,0 }, 100),
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
		std::future<void> pass0 = std::async(std::launch::async, [&]()
			{
				oceanRenderPass->recordCommand();
			});

		std::future<void> pass1 = std::async(std::launch::async, [&]()
			{
				postProcessPass->recordCommand();
			});

		pass0.get();

		ID3D11CommandList* cmd0 = oceanRenderPass->finishRecord();

		CommandListArray::pushCommandList(cmd0);

		pass1.get();

		ID3D11CommandList* cmd1 = postProcessPass->finishRecord();

		CommandListArray::pushCommandList(cmd1);
	}


};