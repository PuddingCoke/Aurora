#include<Aurora/Aurora.h>

#include"MyGame.h"

using EngineUsage = Configuration::EngineUsage;

int main(int argc, const char* argv[])
{
	Configuration config(1920, 1080, L"Aurora Test", 1, EngineUsage::AnimationRender, true);
	if (!Aurora::get().iniEngine(config, argc, argv))
	{
		Aurora::get().iniGame(new MyGame());
	}
}