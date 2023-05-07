#pragma once

#include<Aurora/Core/RenderAPI.h>

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

	const unsigned int materialIndex;

	const unsigned int indexCount;

	Model(const aiScene* const scene, const aiMesh* const mesh) :
		materialIndex(mesh->mMaterialIndex),
		indexCount(mesh->mNumFaces * 3u)
	{
		std::vector<Vertex> vertices(mesh->mNumVertices);
		std::vector<UINT> indices(mesh->mNumFaces * 3u);

		bool hasUV, hasTangent;

		hasUV = mesh->HasTextureCoords(0);
		hasTangent = mesh->HasTangentsAndBitangents();

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
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

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			indices[i * 3u] = mesh->mFaces[i].mIndices[0];
			indices[i * 3u + 1u] = mesh->mFaces[i].mIndices[1];
			indices[i * 3u + 2u] = mesh->mFaces[i].mIndices[2];
		}


		vertexBuffer = new Buffer(sizeof(Vertex) * mesh->mNumVertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE, vertices.data());
		indexBuffer = new Buffer(sizeof(UINT) * indices.size(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE, indices.data());
	}

	~Model()
	{
		delete vertexBuffer;
		delete indexBuffer;
	}

	void draw()
	{
		RenderAPI::get()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		RenderAPI::get()->IASetVertexBuffer(0, { vertexBuffer }, { sizeof(Vertex) }, { 0 });
		RenderAPI::get()->DrawIndexed(indexCount, 0, 0);
	}

private:

	//pos uv normal tangent bitangent
	Buffer* vertexBuffer;

	Buffer* indexBuffer;

};