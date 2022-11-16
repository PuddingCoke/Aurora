#pragma once

#include"Material.h"
#include"Model.h"

class Scene
{
public:

	Scene(const std::string& filePath)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filePath, aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate |
			aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			materials.emplace_back(new Material(scene, i));
		}

		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			std::string meshName = scene->mMeshes[i]->mName.C_Str();

			if (materials[scene->mMeshes[i]->mMaterialIndex]->type != Material::Trans)
			{
				opaqueModel.push_back(new Model(scene, scene->mMeshes[i]));
			}
			else
			{
				transparentModel.push_back(new Model(scene, scene->mMeshes[i]));
			}

		}

		struct Light
		{
			DirectX::XMFLOAT4 position;
			DirectX::XMFLOAT4 color;
			float radius;
			float quadraticFalloff;
			float linearFalloff;
			float v0;
		} lights[22]{};

		for (unsigned int i = 2; i < scene->mNumLights; i++)
		{
			lights[i - 2].position = { scene->mLights[i]->mPosition.x,scene->mLights[i]->mPosition.y - 1.f, scene->mLights[i]->mPosition.z,1.f };
			lights[i - 2].color = { 1.f,1.f,1.f,1.f };
			lights[i - 2].radius = 30.f;
		}

		lightBuffer = new Buffer(sizeof(Light) * 22, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, lights);
	}

	void draw(Shader* vertexShader, Shader* pixelHasTex, Shader* pixelNoTex, Shader* pixelTrans)
	{
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetBlendState(nullptr);

		vertexShader->use();

		for (Model* model : opaqueModel)
		{
			switch (materials[model->materialIndex]->type)
			{
			case Material::Normal:
				pixelHasTex->use();
				break;
			case Material::NoTex:
				pixelNoTex->use();
				break;
			}

			materials[model->materialIndex]->use();
			model->draw();
		}

		pixelTrans->use();
		RenderAPI::get()->OMSetBlendState(States::defBlendState);

		for (Model* model : transparentModel)
		{
			materials[model->materialIndex]->use();
			model->draw();
		}

		RenderAPI::get()->OMSetBlendState(nullptr);

	}

	~Scene()
	{
		if (lightBuffer)
		{
			delete lightBuffer;
		}

		for (unsigned int i = 0; i < materials.size(); i++)
		{
			delete materials[i];
		}

		for (unsigned int i = 0; i < opaqueModel.size(); i++)
		{
			delete opaqueModel[i];
		}

		for (unsigned int i = 0; i < transparentModel.size(); i++)
		{
			delete transparentModel[i];
		}
	}

	Buffer* lightBuffer;

private:

	std::vector<Material*> materials;

	std::vector<Model*> opaqueModel;

	std::vector<Model*> transparentModel;

};