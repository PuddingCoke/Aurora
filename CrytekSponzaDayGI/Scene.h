#pragma once

#include"Material.h"
#include"Model.h"

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

const std::string assetPath = "E:/Assets/Sponza";

class Scene
{
public:

	static constexpr UINT stride = sizeof(Vertex);

	static constexpr UINT offset = 0;

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

		std::vector<Vertex> vertices;

		UINT startVertexLocation = 0;

		UINT vertexCount = 0;

		for (unsigned int j = 0; j < scene->mNumMeshes; j++)
		{
			const bool hasUV = scene->mMeshes[j]->HasTextureCoords(0);
			const bool hasTangent = scene->mMeshes[j]->HasTangentsAndBitangents();

			vertexCount = scene->mMeshes[j]->mNumVertices;

			for (unsigned int i = 0; i < scene->mMeshes[j]->mNumVertices; i++)
			{
				Vertex vert;

				vert.pos = DirectX::XMFLOAT3(scene->mMeshes[j]->mVertices[i].x, scene->mMeshes[j]->mVertices[i].y, scene->mMeshes[j]->mVertices[i].z);
				if (hasUV)
				{
					vert.uv = DirectX::XMFLOAT2(scene->mMeshes[j]->mTextureCoords[0][i].x, scene->mMeshes[j]->mTextureCoords[0][i].y);
				}
				else
				{
					vert.uv = DirectX::XMFLOAT2(0.f, 0.f);
				}
				vert.normal = DirectX::XMFLOAT3(scene->mMeshes[j]->mNormals[i].x, scene->mMeshes[j]->mNormals[i].y, scene->mMeshes[j]->mNormals[i].z);
				if (hasTangent)
				{
					vert.tangent = DirectX::XMFLOAT3(scene->mMeshes[j]->mTangents[i].x, scene->mMeshes[j]->mTangents[i].y, scene->mMeshes[j]->mTangents[i].z);
					vert.bitangent = DirectX::XMFLOAT3(scene->mMeshes[j]->mBitangents[i].x, scene->mMeshes[j]->mBitangents[i].y, scene->mMeshes[j]->mBitangents[i].z);
				}
				else
				{
					vert.tangent = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
					vert.bitangent = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
				}

				vertices.push_back(vert);
			}

			models.push_back(Model(scene->mMeshes[j]->mMaterialIndex, vertexCount, startVertexLocation));

			startVertexLocation += vertexCount;
		}

		modelBuffer = new VertexBuffer(sizeof(Vertex) * vertices.size(), D3D11_USAGE_IMMUTABLE, vertices.data());

	}

	~Scene()
	{
		delete modelBuffer;

		for (unsigned int i = 0; i < materials.size(); i++)
		{
			delete materials[i];
		}
	}

	void render(Shader* const vertexShader, Shader* const pixelShader)
	{
		ImCtx::get()->BindShader(vertexShader);
		ImCtx::get()->GSUnbindShader();
		ImCtx::get()->BindShader(pixelShader);
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->IASetVertexBuffer(0, { modelBuffer }, { stride }, { offset });
		for (unsigned int i = 0; i < models.size(); i++)
		{
			materials[models[i].materialIndex]->use();
			models[i].draw();
		}
	}

	void renderCube(Shader* const vertexShader, Shader* const pixelShader)
	{
		ImCtx::get()->BindShader(vertexShader);
		ImCtx::get()->GSUnbindShader();
		ImCtx::get()->BindShader(pixelShader);
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->IASetVertexBuffer(0, { modelBuffer }, { stride }, { offset });
		for (unsigned int i = 0; i < models.size(); i++)
		{
			materials[models[i].materialIndex]->use();
			models[i].drawInstance();
		}
	}

	void renderGeometry(Shader* const vertexShader)
	{
		ImCtx::get()->BindShader(vertexShader);
		ImCtx::get()->GSUnbindShader();
		ImCtx::get()->PSUnbindShader();
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->IASetVertexBuffer(0, { modelBuffer }, { stride }, { offset });
		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i].draw();
		}
	}

	std::vector<Material*> materials;

	std::vector<Model> models;

	VertexBuffer* modelBuffer;

};
