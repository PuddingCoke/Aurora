#include<Aurora/A3D/Model.h>

Model* Model::create(const std::string& path, unsigned int& numModels)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw importer.GetErrorString();
	}

	numModels = scene->mNumMeshes;

	Model* models = new Model[scene->mNumMeshes];

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		models[i] = Model(scene, mesh);
	}

	return models;
}

Model::~Model()
{

}

void Model::draw() const
{
	Graphics::context->PSSetConstantBuffers(3, 1, materialBuffer.GetAddressOf());

	Graphics::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::context->IASetVertexBuffers(0, 1, meshBuffer.GetAddressOf(), &stride, &offset);

	Graphics::context->Draw(vertexNum, 0);
}

void Model::ini()
{
	D3D11_INPUT_ELEMENT_DESC layout[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
}

Model::Model() :
	vertexNum(0)
{
}

Model::Model(const aiScene* const scene, const aiMesh* const mesh)
{
	//vertex texCoord normal
	std::vector<float> vertices;

	vertexNum = mesh->mNumVertices;

	if (mesh->mTextureCoords[0] == nullptr)
	{
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			//32bytes
			vertices.push_back(mesh->mVertices[i].x);
			vertices.push_back(mesh->mVertices[i].y);
			vertices.push_back(mesh->mVertices[i].z);
			vertices.push_back(0);
			vertices.push_back(0);
			vertices.push_back(mesh->mNormals[i].x);
			vertices.push_back(mesh->mNormals[i].y);
			vertices.push_back(mesh->mNormals[i].z);
		}
	}
	else
	{
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			//32bytes
			vertices.push_back(mesh->mVertices[i].x);
			vertices.push_back(mesh->mVertices[i].y);
			vertices.push_back(mesh->mVertices[i].z);
			vertices.push_back(mesh->mTextureCoords[0][i].x);
			vertices.push_back(mesh->mTextureCoords[0][i].y);
			vertices.push_back(mesh->mNormals[i].x);
			vertices.push_back(mesh->mNormals[i].y);
			vertices.push_back(mesh->mNormals[i].z);
		}
	}

	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth = sizeof(float) * vertices.size();
		bd.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA subResource = {};
		subResource.pSysMem = vertices.data();

		Graphics::device->CreateBuffer(&bd, &subResource, meshBuffer.ReleaseAndGetAddressOf());
	}

	{
		MaterialProperty properties = {};

		aiColor3D color;

		scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_AMBIENT, color);

		properties.ambientColor = DirectX::XMFLOAT3(color.r, color.g, color.b);

		scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		properties.diffuseColor = DirectX::XMFLOAT3(color.r, color.g, color.b);

		scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_SPECULAR, color);

		properties.specularColor = DirectX::XMFLOAT3(color.r, color.g, color.b);

		scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_EMISSIVE, color);

		properties.emmisiveColor = DirectX::XMFLOAT3(color.r, color.g, color.b);

		ai_real factor;

		scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_SHININESS, factor);

		properties.shininess = factor;

		scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_REFRACTI, factor);

		properties.refraction = factor;

		scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_OPACITY, factor);

		properties.opacity = factor;

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(MaterialProperty);
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA subresource = {};
		subresource.pSysMem = &properties;

		Graphics::device->CreateBuffer(&bd, &subresource, materialBuffer.ReleaseAndGetAddressOf());
	}

}
