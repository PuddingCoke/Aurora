#pragma once

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include"Material.h"
#include"Model.h"

#include<vector>

const std::string assetPath = "D:/Assets/SponzaPBR";

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

		const aiScene* scene = importer.ReadFile(filePath, aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs| aiProcess_Triangulate | 
			aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			aiString texturePath;
			std::cout << scene->mMaterials[i]->GetName().C_Str() << "\n";

			std::string basePath = "";
			std::string metallicRoughnessPath = "";
			std::string normalPath = "";

			if (scene->mMaterials[i]->GetTextureCount(aiTextureType_BASE_COLOR))
			{
				scene->mMaterials[i]->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);
				basePath = texturePath.C_Str();
			}

			if (scene->mMaterials[i]->GetTextureCount(aiTextureType_UNKNOWN))
			{
				scene->mMaterials[i]->GetTexture(aiTextureType_UNKNOWN, 0, &texturePath);
				metallicRoughnessPath = texturePath.C_Str();
			}

			if (scene->mMaterials[i]->GetTextureCount(aiTextureType_NORMALS))
			{
				scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
				normalPath = texturePath.C_Str();
			}

			std::cout << basePath << "\n" << metallicRoughnessPath << "\n" << normalPath << "\n";

			materials.push_back(new Material(basePath, metallicRoughnessPath, normalPath));
		}

		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			if (materials[scene->mMeshes[i]->mMaterialIndex]->metallicRoughness != nullptr)
			{
				models.push_back(new Model(scene, scene->mMeshes[i], true));
			}
			else if (materials[scene->mMeshes[i]->mMaterialIndex]->base == nullptr)
			{
				models.push_back(new Model(scene, scene->mMeshes[i], false));
			}
		}

	}

	void drawShadow()
	{
		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->draw();
		}

	}

	void draw(Shader* const shader,Shader* shaderNoTex)
	{
		for (unsigned int i = 0; i < models.size(); i++)
		{
			if (models[i]->hasTexture)
			{
				shader->use();
				materials[models[i]->materialIndex]->use();
				models[i]->draw();
			}
			else
			{
				shaderNoTex->use();
				Renderer::context->PSSetConstantBuffers(3, 1, models[i]->materialBuffer.GetAddressOf());
				models[i]->draw();
			}
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

private:

	std::vector<Material*> materials;

	std::vector<Model*> models;
};