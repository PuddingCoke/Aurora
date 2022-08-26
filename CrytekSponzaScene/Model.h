#pragma once

#ifndef _MODEL_H_
#define _MODEL_H_

#include<Aurora/Graphics.h>

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
	ComPtr<ID3D11Buffer> modelBuffer;

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

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(Vertex) * vertexCount;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		
		D3D11_SUBRESOURCE_DATA subresource = {};
		subresource.pSysMem = vertices.data();

		Renderer::device->CreateBuffer(&bd, &subresource, modelBuffer.ReleaseAndGetAddressOf());
	}

	void draw()
	{
		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Renderer::context->IASetVertexBuffers(0, 1, modelBuffer.GetAddressOf(), &stride, &offset);

		Renderer::context->Draw(vertexCount, 0);
	}

};

#endif // !_MODEL_H_
