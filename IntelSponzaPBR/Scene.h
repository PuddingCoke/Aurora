#pragma once

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

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

		const aiScene* scene = importer.ReadFile(filePath, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			std::cout << i << "\n";
			aiString texturePath;
			std::cout << scene->mMaterials[i]->GetName().C_Str() << "\n";

			scene->mMaterials[i]->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);
			std::cout << texturePath.C_Str() << "\n";

			scene->mMaterials[i]->GetTexture(aiTextureType_UNKNOWN, 0, &texturePath);
			std::cout << texturePath.C_Str() << "\n";

			scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
			std::cout << texturePath.C_Str() << "\n";
		}

	}

	~Scene()
	{
	}

};