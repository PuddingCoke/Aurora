#pragma once

#include<Aurora/Texture2D.h>

class Material
{
public:

	Material() = delete;

	Material(const std::string& basePath, const std::string& metallicRoughnessPath, std::string& normalPath) :
		base(Texture2D::create(basePath)),
		metallicRoughness(Texture2D::create(metallicRoughnessPath)),
		normal(Texture2D::create(normalPath))
	{

	}

	~Material()
	{
		delete base;
		delete metallicRoughness;
		delete normal;
	}

	void use()
	{
		base->setSRV(0);
		metallicRoughness->setSRV(1);
		normal->setSRV(2);
	}

	Texture2D* const base;
	Texture2D* const metallicRoughness;
	Texture2D* const normal;

};