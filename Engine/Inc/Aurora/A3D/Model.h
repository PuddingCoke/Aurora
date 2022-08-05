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

	static Model* create(const std::string& path, unsigned int& numMesh);
	
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

};

#endif // !_MODEL_H_
