#pragma once

#include<Aurora/EngineAPI/ImCtx.h>
#include<Aurora/Resource/ResourceTexture.h>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

const std::string materialBasePath = "E:/Assets/SuperSponza/";

class Material
{
public:

	enum Type
	{
		Normal,
		NoTex,
		Trans
	};

	Material() = delete;

	Material(const aiScene* scene, unsigned int& idx)
	{
		aiString texturePath;

		std::string basePath = "";
		std::string metallicRoughnessPath = "";
		std::string normalPath = "";


		if (scene->mMaterials[idx]->GetTextureCount(aiTextureType_BASE_COLOR))
		{
			scene->mMaterials[idx]->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);
			basePath = texturePath.C_Str();
			baseColor = new ResourceTexture(materialBasePath + basePath);
		}

		if (scene->mMaterials[idx]->GetTextureCount(aiTextureType_UNKNOWN))
		{
			scene->mMaterials[idx]->GetTexture(aiTextureType_UNKNOWN, 0, &texturePath);
			metallicRoughnessPath = texturePath.C_Str();
			metallicRoughness = new ResourceTexture(materialBasePath + metallicRoughnessPath);
		}

		if (scene->mMaterials[idx]->GetTextureCount(aiTextureType_NORMALS))
		{
			scene->mMaterials[idx]->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
			normalPath = texturePath.C_Str();
			normal = new ResourceTexture(materialBasePath + normalPath);
		}

		aiColor4D color;
		ai_real metallic;
		ai_real roughness;

		scene->mMaterials[idx]->Get(AI_MATKEY_BASE_COLOR, color);
		scene->mMaterials[idx]->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
		scene->mMaterials[idx]->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);

		if (basePath != "" && metallicRoughnessPath != "")
		{
			type = Normal;
		}
		else if (basePath == "")
		{
			type = NoTex;
		}
		else
		{
			type = Trans;
		}

		/*std::cout << "material name " << scene->mMaterials[idx]->GetName().C_Str() << "\n";
		std::cout << "baseColor " << basePath << "\n";
		std::cout << "metallicRoughness " << metallicRoughnessPath << "\n";
		std::cout << "normal " << normalPath << "\n";
		std::cout << "baseColor " << color.r << " " << color.g << " " << color.b << "\n";
		std::cout << "metalness " << metallic << "\n";
		std::cout << "roughness " << roughness << "\n";
		std::cout << "type " << type << "\n";
		std::cout << "\n";*/

		struct MaterialProp
		{
			DirectX::XMFLOAT4 baseColor;
			float roughness;
			float metallic;
			DirectX::XMFLOAT2 padding;
		}prop;

		prop.baseColor = { color.r,color.g,color.b,1.f };
		prop.roughness = roughness;
		prop.metallic = metallic;

		materialBuffer = new ConstantBuffer(sizeof(MaterialProp), D3D11_USAGE_IMMUTABLE, &prop);
	}

	~Material()
	{
		if (materialBuffer)
		{
			delete materialBuffer;
		}

		if (baseColor)
		{
			delete baseColor;
		}

		if (metallicRoughness)
		{
			delete metallicRoughness;
		}

		if (normal)
		{
			delete normal;
		}
	}

	void use()
	{
		ImCtx::get()->PSSetConstantBuffer({ materialBuffer }, 1);

		switch (type)
		{
		case Normal:
			ImCtx::get()->PSSetSRV({ baseColor,metallicRoughness,normal }, 0);
			break;
		case Trans:
			ImCtx::get()->PSSetSRV({ baseColor }, 0);
		}
	}

	ConstantBuffer* materialBuffer;

	ResourceTexture* baseColor;
	ResourceTexture* metallicRoughness;
	ResourceTexture* normal;

	Type type;

};