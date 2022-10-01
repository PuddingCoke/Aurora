#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A3D/FPSCamera.h>

class MyGame :public Game
{
public:

	FPSCamera camera;

	MyGame() :
		camera({ 0,0,0 }, { 1,0,0 }, { 0,1,0 }, 40, 3)
	{
		camera.registerEvent();
	}

	~MyGame()
	{
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		Renderer::clearDefRTV(DirectX::Colors::CadetBlue);
		Renderer::setDefRTV();
	}


};