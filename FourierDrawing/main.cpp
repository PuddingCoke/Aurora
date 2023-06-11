#include<Aurora/Aurora.h>

#include"MyGame.h"

using EngineUsage = Configuration::EngineUsage;

int main(int argc, const char* argv[])
{
	Configuration config(1000, 1000, L"Aurora Test", 8, EngineUsage::Normal, true);
	if (!Aurora::get().iniEngine(config, argc, argv))
	{
		Aurora::get().iniGame(new MyGame());
	}
}