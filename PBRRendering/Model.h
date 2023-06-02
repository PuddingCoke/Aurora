#pragma once

#include<Aurora/EngineAPI/RenderAPI.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
};

class Model
{
public:

	const unsigned int vertexNum;

	Model(const aiScene* const scene,const aiMesh* const mesh) :
		vertexNum(mesh->mNumVertices)
	{
		{

			std::vector<Vertex> vertices;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				vertices.push_back(Vertex{ {mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z},{mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z} });
			}

			vertexBuffer = new VertexBuffer(sizeof(Vertex) * vertices.size(), D3D11_USAGE_IMMUTABLE, vertices.data());

		}

		{
			struct MaterialProp
			{
				DirectX::XMFLOAT4 ambientColor;
				DirectX::XMFLOAT4 diffuseColor;
				DirectX::XMFLOAT4 specularColor;
			}prop;

			aiColor3D color;

			scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_AMBIENT, color);

			prop.ambientColor = DirectX::XMFLOAT4(color.r, color.g, color.b, 1.f);

			scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

			prop.diffuseColor = DirectX::XMFLOAT4(color.r, color.g, color.b, 1.f);

			scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_SPECULAR, color);

			prop.specularColor = DirectX::XMFLOAT4(color.r, color.g, color.b, 1.f);

			materialBuffer = new ConstantBuffer(sizeof(MaterialProp), D3D11_USAGE_IMMUTABLE, &prop);
		}

	}

	void draw()
	{
		RenderAPI::get()->PSSetConstantBuffer({ materialBuffer }, 2);
		RenderAPI::get()->IASetVertexBuffer(0, { vertexBuffer }, { sizeof(Vertex) }, { 0 });
		RenderAPI::get()->Draw(vertexNum, 0);
	}

	~Model()
	{
		delete vertexBuffer;
		delete materialBuffer;
	}

private:

	VertexBuffer* vertexBuffer;
	ConstantBuffer* materialBuffer;

};