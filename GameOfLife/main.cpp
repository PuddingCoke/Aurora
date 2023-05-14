#include<Aurora/Aurora.h>

#include"MyGame.h"

using CameraType = Configuration::CameraType;
using EngineUsage = Configuration::EngineUsage;

int main(int argc, const char* argv[])
{
	Configuration config(1920, 1080, L"Aurora Test", CameraType::Orthogonal, 4, EngineUsage::Normal, true);
	if (!Aurora::get().iniEngine(config))
	{
		Aurora::get().iniGame(new MyGame());
	}
}