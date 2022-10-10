#pragma once

#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include<Aurora/Graphics.h>
#include<Aurora/Texture2D.h>

class Material
{
public:

	Material() = delete;

	Material(const std::string& diffusePath, const std::string& specularPath, std::string& normalPath) :
		diffuse(Texture2D::create(diffusePath)), 
		specular(Texture2D::create(specularPath)), 
		normal(Texture2D::create(normalPath))
	{

	}

	~Material()
	{
		delete diffuse;
		delete specular;
		delete normal;
	}

	void use()
	{
		diffuse->PSSetSRV(0);
		specular->PSSetSRV(1);
		normal->PSSetSRV(2);
	}

	Texture2D* const diffuse;
	Texture2D* const specular;
	Texture2D* const normal;

};

#endif // !_MATERIAL_H_
