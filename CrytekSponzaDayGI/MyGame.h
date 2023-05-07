#pragma once

#include<Aurora/Game.h>

#include<Aurora/Core/Camera/FPSCamera.h>

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
		globalIllumination.imGUICall();
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);

		globalIllumination.update(dt);
	}

	void render()
	{
		globalIllumination.render();
	}


};