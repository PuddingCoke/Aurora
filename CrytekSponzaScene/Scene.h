#pragma once

#ifndef _SCENE_H_
#define _SCENE_H_

#include"Material.h"
#include"Model.h"

class Scene
{
public:

	static Scene* create(const std::string& filePath)
	{
		return new Scene(filePath);
	}

	Scene(const std::string& filePath)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

		const std::string assetPath = "scene";

		//œ»‘ÿ»Îmaterial
		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			aiString texturePath;
			std::string diffusePath;
			std::string specularPath;
			std::string normalPath;

			if (scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
				diffusePath = assetPath + texturePath.C_Str();
			}
			else
			{
				diffusePath = assetPath + "/sponza/dummy.dds";
			}

			if (scene->mMaterials[i]->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				scene->mMaterials[i]->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
				specularPath = assetPath + texturePath.C_Str();
			}
			else
			{
				specularPath = assetPath + "/sponza/dummy_specular.dds";
			}

			if (scene->mMaterials[i]->GetTextureCount(aiTextureType_NORMALS) > 0)
			{
				scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
				normalPath = assetPath + texturePath.C_Str();
			}
			else
			{
				normalPath = assetPath + "/sponza/dummy_ddn.dds";
			}

			materials.push_back(new Material(diffusePath, specularPath, normalPath));
		}

		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			models.push_back(new Model(scene, scene->mMeshes[i]));
		}

	}

	~Scene()
	{
		for (unsigned int i = 0; i < materials.size(); i++)
		{
			delete materials[i];
		}
		for (unsigned int i = 0; i < models.size(); i++)
		{
			delete models[i];
		}
	}

	void draw()
	{
		for (unsigned int i = 0; i < models.size(); i++)
		{
			materials[models[i]->materialIndex]->use();
			models[i]->draw();
		}
	}

	std::vector<Material*> materials;

	std::vector<Model*> models;

};

#endif // !_SCENE_H_
