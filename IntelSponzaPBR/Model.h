#pragma once

#include<Aurora/Renderer.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<vector>

class Model
{
public:

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 bitangent;
	};

	static constexpr unsigned int stride = sizeof(Vertex);

	static constexpr unsigned int offset = 0;

	ComPtr<ID3D11Buffer> modelBuffer;

	ComPtr<ID3D11Buffer> indexBuffer;

	ComPtr<ID3D11Buffer> materialBuffer;

	const unsigned int materialIndex;

	const unsigned int indexCount;

	const bool hasTexture;

	Model(const aiScene* const scene, const aiMesh* const mesh, const bool& hasTexture) :
		materialIndex(mesh->mMaterialIndex),
		indexCount(mesh->mNumFaces * 3u),
		hasTexture(hasTexture)
	{
		std::vector<Vertex> vertices(mesh->mNumVertices);
		std::vector<UINT> indices(mesh->mNumFaces * 3u);

		if (hasTexture)
		{
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				vertices[i].pos = DirectX::XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertices[i].uv = DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				vertices[i].normal = DirectX::XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				vertices[i].tangent = DirectX::XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
				vertices[i].bitangent = DirectX::XMFLOAT3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			}
		}
		else
		{
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				vertices[i].pos = DirectX::XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertices[i].uv = DirectX::XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
				vertices[i].normal = DirectX::XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				vertices[i].tangent = DirectX::XMFLOAT3(0, 1, 0);
				vertices[i].bitangent = DirectX::XMFLOAT3(0, 1, 0);
			}

			struct MaterialProp
			{
				DirectX::XMFLOAT4 baseColor;
				float metalness;
				float roughness;
				DirectX::XMFLOAT2 padding;
			}property{};

			aiColor4D color;
			ai_real real;

			scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_BASE_COLOR, color);
			property.baseColor = DirectX::XMFLOAT4(color.r, color.g, color.b, color.a);
			scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_METALLIC_FACTOR, real);
			property.metalness = real;
			scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_ROUGHNESS_FACTOR, real);
			property.roughness = real;

			D3D11_BUFFER_DESC cbd = {};
			cbd.ByteWidth = sizeof(MaterialProp);
			cbd.Usage = D3D11_USAGE_IMMUTABLE;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			D3D11_SUBRESOURCE_DATA constantData = {};
			constantData.pSysMem = &property;

			Renderer::device->CreateBuffer(&cbd, &constantData, materialBuffer.ReleaseAndGetAddressOf());
		}



		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			indices[i * 3u] = mesh->mFaces[i].mIndices[0];
			indices[i * 3u + 1u] = mesh->mFaces[i].mIndices[1];
			indices[i * 3u + 2u] = mesh->mFaces[i].mIndices[2];
		}

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(Vertex) * mesh->mNumVertices;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subresource = {};
		subresource.pSysMem = vertices.data();

		D3D11_BUFFER_DESC indexDesc = {};
		indexDesc.ByteWidth = sizeof(UINT) * indices.size();
		indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubresource = {};
		indexSubresource.pSysMem = indices.data();

		Renderer::device->CreateBuffer(&bd, &subresource, modelBuffer.ReleaseAndGetAddressOf());
		Renderer::device->CreateBuffer(&indexDesc, &indexSubresource, indexBuffer.ReleaseAndGetAddressOf());
	}

	void draw()
	{
		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Renderer::context->IASetVertexBuffers(0, 1, modelBuffer.GetAddressOf(), &stride, &offset);
		Renderer::context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
		Renderer::context->DrawIndexed(indexCount, 0u, 0u);
	}

};