#pragma once

#include<Aurora/RenderAPI.h>
#include<Aurora/ResourceTexture.h>

class Material
{
public:

	Material() = delete;

	Material(const std::string& diffusePath, const std::string& specularPath, std::string& normalPath) :
		diffuse(new ResourceTexture(diffusePath)),
		specular(new ResourceTexture(specularPath)),
		normal(new ResourceTexture(normalPath))
	{

	}

	~Material()
	{
		delete diffuse;
		delete specular;
		delete normal;
	}

	void use() const
	{
		RenderAPI::get()->PSSetSRV({ diffuse,specular,normal }, 0);
	}

	ResourceTexture* const diffuse;
	ResourceTexture* const specular;
	ResourceTexture* const normal;

};
