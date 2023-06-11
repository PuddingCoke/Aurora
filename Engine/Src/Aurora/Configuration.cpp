#include<Aurora/Configuration.h>

Configuration::Configuration(const int& width, const int& height, const std::wstring& title, const unsigned int& msaaLevel, const EngineUsage& usage, const bool& enableImGui) :
	width(width), height(height), title(title), usage(usage), msaaLevel(msaaLevel), enableImGui(enableImGui)
{
}
