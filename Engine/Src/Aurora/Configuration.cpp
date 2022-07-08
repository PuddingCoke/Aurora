#include<Aurora/Configuration.h>

Configuration::Configuration(const HINSTANCE& hInstance, const int& width, const int& height, const std::wstring& title, const CameraType& cameraType, const unsigned int& msaaLevel, const EngineUsage& usage,const bool& enableDebug) :
	hInstance(hInstance), width(width), height(height), title(title), usage(usage), cameraType(cameraType), msaaLevel(msaaLevel),enableDebug(enableDebug)
{
}
