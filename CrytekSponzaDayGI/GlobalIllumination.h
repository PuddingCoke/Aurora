#pragma once

#include<Aurora/ComputeTexture.h>

class GlobalIllumination
{
public:

	GlobalIllumination():
		irradianceCoeff(new ComputeTexture(9,1,DXGI_FORMAT_R16G16B16A16_FLOAT,1024)),
		depthOctahedralMap(new ComputeTexture(16, 16, DXGI_FORMAT_R16G16_FLOAT, 1024))
	{

	}

	~GlobalIllumination()
	{
		delete irradianceCoeff;
		delete depthOctahedralMap;
	}

private:

	ComputeTexture* irradianceCoeff;

	ComputeTexture* depthOctahedralMap;



};