#pragma once

#include<Aurora/Game.h>
#include<Aurora/PBR/RealShading.h>

#include"Scene.h"

class MyGame :public Game
{
public:

	Scene* scene;

	MyGame():
		scene(Scene::create(assetPath+"/Main/NewSponza_Main_Blender_glTF.gltf"))
	{
	}

	~MyGame()
	{
		delete scene;
	}

	void update(const float& dt) override
	{

	}

	void render()
	{
		
	}


};