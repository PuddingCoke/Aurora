#pragma once

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define NOMINMAX

#include<Windows.h>
#include<string>

class Configuration
{
public:

	const enum class EngineUsage
	{
		Normal,
		Wallpaper,
		AnimationRender
	} usage;

	const enum class CameraType
	{
		Orthogonal,
		Perspective
	} cameraType;

	HINSTANCE hInstance;

	const int width;

	const int height;

	const std::wstring title;

	const unsigned int msaaLevel;

	const bool enableDebug;
	
	Configuration(const HINSTANCE& hInstance, const int& width, const int& height, const std::wstring& title, const CameraType& cameraType, const unsigned int& msaaLevel = 1, const EngineUsage& usage = EngineUsage::Normal, const bool& enableDebug = false);

};

#endif // !_CONFIGURATION_H_
