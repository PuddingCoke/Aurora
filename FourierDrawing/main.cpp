#include<Aurora/Aurora.h>

#include"MyGame.h"

using CameraType = Configuration::CameraType;
using EngineUsage = Configuration::EngineUsage;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	Configuration config(hInstance, 400, 400, L"Aurora Test", CameraType::Orthogonal, 4, EngineUsage::AnimationRender);
	if (!Aurora::get().iniEngine(config))
	{
		Aurora::get().iniGame(new MyGame());
	}
}