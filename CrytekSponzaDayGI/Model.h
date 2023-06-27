#pragma once

#include<Aurora/EngineAPI/ImCtx.h>

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

	const UINT materialIndex;

	const UINT vertexCount;

	const UINT startVertexLocation;

	Model(const UINT& materialIndex, const UINT& vertexCount, const UINT& startVertexLocation) :
		materialIndex(materialIndex),
		vertexCount(vertexCount),
		startVertexLocation(startVertexLocation)
	{
	}

	Model(const Model& m) :
		materialIndex(m.materialIndex), vertexCount(m.vertexCount), startVertexLocation(m.startVertexLocation)
	{

	}

	~Model()
	{
	}

	void draw(GraphicsContext* const ctx)
	{
		ctx->Draw(vertexCount, startVertexLocation);
	}

	void drawInstance(GraphicsContext* const ctx)
	{
		ctx->DrawInstanced(vertexCount, 6, startVertexLocation, 0);
	}

};
