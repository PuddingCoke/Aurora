#include<Aurora/Aurora.h>

#include"MyGame.h"

using CameraType = Configuration::CameraType;
using EngineUsage = Configuration::EngineUsage;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nCmdShow)
{
	Configuration config(hInstance, 1920, 1080, L"Aurora Test", CameraType::Perspective, 1, EngineUsage::Normal, true);
	if (!Aurora::get().iniEngine(config))
	{
		Aurora::get().iniGame(new MyGame());
	}

}