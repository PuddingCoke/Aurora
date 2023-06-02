#pragma once

#include<Aurora/EngineAPI/RenderAPI.h>

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

	Model(const UINT& materialIndex,const UINT& vertexCount,const UINT& startVertexLocation) :
		materialIndex(materialIndex),
		vertexCount(vertexCount),
		startVertexLocation(startVertexLocation)
	{
	}

	~Model()
	{
	}

	void draw()
	{
		RenderAPI::get()->Draw(vertexCount, startVertexLocation);
	}

	void drawInstance()
	{
		RenderAPI::get()->DrawInstanced(vertexCount, 6, startVertexLocation, 0);
	}

};
