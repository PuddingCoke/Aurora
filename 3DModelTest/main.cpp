#include<Aurora/Aurora.h>

#include"MyGame.h"

using CameraType = Configuration::CameraType;
using EngineUsage = Configuration::EngineUsage;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	Configuration config(hInstance, 1920, 1080, L"Aurora Test", CameraType::Perspective, 4, EngineUsage::Normal, true);
	if (!Aurora::get().iniEngine(config))
	{
		Aurora::get().iniGame(new MyGame());
	}
}