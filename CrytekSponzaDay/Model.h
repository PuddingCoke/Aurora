#pragma once

#ifndef _MODEL_H_
#define _MODEL_H_

#include<Aurora/Renderer.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<vector>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 bitangent;
};

class Model
{
public:

	static constexpr unsigned int stride = sizeof(Vertex);

	static constexpr unsigned int offset = 0;

	//pos uv normal tangent bitangent
	Buffer* modelBuffer;

	const unsigned int materialIndex;

	const unsigned int vertexCount;

	Model(const aiScene* const scene, const aiMesh* const mesh):
		materialIndex(mesh->mMaterialIndex),
		vertexCount(mesh->mNumVertices)
	{
		std::vector<Vertex> vertices(mesh->mNumVertices);

		bool hasUV, hasTangent;

		hasUV = mesh->HasTextureCoords(0);
		hasTangent = mesh->HasTangentsAndBitangents();

		for (unsigned int i = 0; i < vertexCount; i++)
		{
			vertices[i].pos = DirectX::XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			if (hasUV)
			{
				vertices[i].uv = DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertices[i].uv = DirectX::XMFLOAT2(0.f, 0.f);
			}
			vertices[i].normal = DirectX::XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			if (hasTangent)
			{
				vertices[i].tangent = DirectX::XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
				vertices[i].bitangent = DirectX::XMFLOAT3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			}
			else
			{
				vertices[i].tangent = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
				vertices[i].bitangent = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
			}
		}

		modelBuffer = new Buffer(sizeof(Vertex) * vertexCount, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE, vertices.data());
	}

	~Model()
	{
		delete modelBuffer;
	}

	void draw()
	{
		RenderAPI::get()->IASetVertexBuffer(0, { modelBuffer }, { stride }, { offset });
		RenderAPI::get()->Draw(vertexCount, 0);
	}

};

#endif // !_MODEL_H_
