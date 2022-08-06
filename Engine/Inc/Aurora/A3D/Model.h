#pragma once

#ifndef _MODEL_H_
#define _MODEL_H_

#include<string>
#include<iostream>
#include<vector>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<Aurora/Graphics.h>

class Model
{
public:

	Model(const Model&) = delete;

	static Model* create(const std::string& path, unsigned int& numModels);
	
	~Model();

	void draw();

	static void ini();

private:

	Model();

	static constexpr unsigned int stride = 32u;

	static constexpr unsigned int offset = 0u;

	static ComPtr<ID3D11InputLayout> modelLayout;

	unsigned int vertexNum;

	Model(const aiScene* const scene,const aiMesh* const mesh);

	//vertex texCoord normal
	ComPtr<ID3D11Buffer> meshBuffer;

	//ambient diffuse ...
	ComPtr<ID3D11Buffer> materialBuffer;

	struct MaterialProperty
	{
		DirectX::XMFLOAT3 ambientColor;
		float shininess;
		DirectX::XMFLOAT3 diffuseColor;
		float refraction;
		DirectX::XMFLOAT3 specularColor;
		float opacity;
		DirectX::XMFLOAT3 emmisiveColor;
		float v3;
	};

};

#endif // !_MODEL_H_
