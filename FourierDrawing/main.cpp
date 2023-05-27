﻿#include<Aurora/Aurora.h>

#include"MyGame.h"

using CameraType = Configuration::CameraType;
using EngineUsage = Configuration::EngineUsage;

int main(int argc, const char* argv[])
{
	Configuration config(1000, 1000, L"Aurora Test", CameraType::Orthogonal, 8, EngineUsage::Normal, true);
	if (!Aurora::get().iniEngine(config,argc,argv))
	{
		Aurora::get().iniGame(new MyGame());
	}
}