#pragma once

#ifndef _SCENE_H_
#define _SCENE_H_

#include"Material.h"
#include"Model.h"

const std::string assetPath = "E:/Assets/Sponza";

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

		const aiScene* scene = importer.ReadFile(filePath, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

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

	void render(Shader* const vertexShader, Shader* const pixelShader)
	{
		vertexShader->use();
		RenderAPI::get()->GSSetShader(nullptr);
		pixelShader->use();

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		for (unsigned int i = 0; i < models.size(); i++)
		{
			const Material* const mat = materials[models[i]->materialIndex];
			RenderAPI::get()->PSSetSRV({ mat->diffuse,mat->specular,mat->normal }, 0);
			models[i]->draw();
		}
	}

	void renderCube(Shader* const vertexShader, Shader* const pixelShader)
	{
		vertexShader->use();
		RenderAPI::get()->GSSetShader(nullptr);
		pixelShader->use();

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		for (unsigned int i = 0; i < models.size(); i++)
		{
			const Material* const mat = materials[models[i]->materialIndex];
			RenderAPI::get()->PSSetSRV({ mat->diffuse,mat->specular }, 0);
			models[i]->drawInstance();
		}
	}

	void renderGeometry(Shader* const vertexShader)
	{
		vertexShader->use();
		RenderAPI::get()->GSSetShader(nullptr);
		RenderAPI::get()->PSSetShader(nullptr);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->draw();
		}
	}

	std::vector<Material*> materials;

	std::vector<Model*> models;

};

#endif // !_SCENE_H_
