#pragma once

#include"Model.h"

class Scene
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	Shader* vertexShader;
	Shader* pixelShader;

	Scene(const std::string& filePath) :
		vertexShader(new Shader("VertexShader.hlsl", ShaderType::Vertex)),
		pixelShader(new Shader("PixelShader.hlsl", ShaderType::Pixel))
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_GenSmoothNormals);

		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			models.push_back(new Model(scene, scene->mMeshes[i]));
		}

		D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		Renderer::get()->createInputLayout(desc, ARRAYSIZE(desc), SHADERDATA(vertexShader), inputLayout.ReleaseAndGetAddressOf());

	}

	void draw()
	{
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->BindShader(vertexShader);
		RenderAPI::get()->BindShader(pixelShader);

		for (unsigned int i = 0; i < models.size(); i++)
		{
			models[i]->draw();
		}

	}

	~Scene()
	{
		delete vertexShader;
		delete pixelShader;

		for (unsigned int i = 0; i < models.size(); i++)
		{
			delete models[i];
		}
	}

private:

	std::vector<Model*> models;

};