﻿#pragma once

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

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

	const int width;

	const int height;

	const std::wstring title;

	const unsigned int msaaLevel;

	const bool enableImGui;

	/// <summary>
	/// width和height在usage为wallpaper时会失效
	/// </summary>
	/// <param name="width">窗口宽</param>
	/// <param name="height">窗口高</param>
	/// <param name="title">标题</param>
	/// <param name="msaaLevel">抗锯齿等级（直接影响到抗锯齿材质的等级）</param>
	/// <param name="usage">用途（标准、壁纸、视频编码）</param>
	/// <param name="enableImGui">ImGui是否开启</param>
	Configuration(const int& width, const int& height, const std::wstring& title, const unsigned int& msaaLevel = 1, const EngineUsage& usage = EngineUsage::Normal, const bool& enableImGui = true);

};

#endif // !_CONFIGURATION_H_
