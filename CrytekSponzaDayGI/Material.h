#pragma once

#include<Aurora/EngineAPI/ImCtx.h>
#include<Aurora/Resource/ResourceTexture.h>

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

	Material(const Material& m) :
		diffuse(new ResourceTexture(*m.diffuse)),
		specular(new ResourceTexture(*m.specular)),
		normal(new ResourceTexture(*m.normal))
	{

	}

	~Material()
	{
		delete diffuse;
		delete specular;
		delete normal;
	}

	void use(GraphicsContext* const ctx) const
	{
		ctx->PSSetSRV({ diffuse,specular,normal }, 0);
	}

	ResourceTexture* const diffuse;
	ResourceTexture* const specular;
	ResourceTexture* const normal;

};
