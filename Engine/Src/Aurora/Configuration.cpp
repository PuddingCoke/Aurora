#include<Aurora/Configuration.h>

Configuration::Configuration(const int& width, const int& height, const std::wstring& title, const CameraType& cameraType, const unsigned int& msaaLevel, const EngineUsage& usage, const bool& enableDebug, const bool& enableImGui) :
	width(width), height(height), title(title), usage(usage), cameraType(cameraType), msaaLevel(msaaLevel), enableDebug(enableDebug), enableImGui(enableImGui)
{
}
