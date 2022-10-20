#include<Aurora/A3D/TextureCube.h>

Shader* TextureCube::shader;

Shader* TextureCube::equirectangularYUP;

Shader* TextureCube::equirectangularZUP;

Shader* TextureCube::equirectangularVShader;

TextureCube* TextureCube::create(std::initializer_list<std::string> texturesPath)
{
	return new TextureCube(texturesPath);
}

TextureCube* TextureCube::createDDSCubeMap(const std::string& texturePath)
{
	return new TextureCube(texturePath);
}

TextureCube* TextureCube::createEquirectangularMap(const std::string& texturePath, const UINT& skyboxResolution, const DirectX::XMFLOAT3& up, const unsigned int& mipLevels)
{
	return new TextureCube(texturePath, skyboxResolution, up, mipLevels);
}

TextureCube::~TextureCube()
{
}

void TextureCube::iniShader()
{
	{
		const std::string source = R"(
cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
}

static const float3 vertices[36] =
{
            // back face
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            // front face
            -1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            // left face
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            // right face
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            // bottom face
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            // top face
            -1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
};

struct VertexOutput
{
    float3 worldPos : POSITION;
    float4 position : SV_Position;
};

VertexOutput main(uint vertexID : SV_VertexID)
{
    float3 position = vertices[vertexID];
    
    float3x3 rotView = (float3x3) view;
    float4 clipPos = mul(float4(mul(position, rotView), 1.0), proj);
    
    VertexOutput output;
    output.worldPos = position;
    output.position = clipPos.xyww;
    
    return output;
}
)";

		std::cout << "skyboxVShader ";

		shader = Shader::fromStr(source, ShaderType::Vertex);

	}

	{
		const std::string source = R"(
SamplerState linearSampler : register(s0);

Texture2D envTexture : register(t0);

static const float2 invAtan = float2(0.1591, 0.3183);

float2 SampleSphjericalMap(float3 v)
{
    float2 uv = float2(atan2(v.x, v.z), asin(-v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float4 main(float3 position : POSITION) : SV_TARGET
{
    float2 uv = SampleSphjericalMap(normalize(position));
    float3 color = envTexture.Sample(linearSampler, uv);
    return float4(color, 1.0);
}
)";

		std::cout << "equirectangularYUP ";

		equirectangularYUP = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
SamplerState linearSampler : register(s0);

Texture2D envTexture : register(t0);

static const float2 invAtan = float2(0.1591, 0.3183);

float2 SampleSphjericalMap(float3 v)
{
    float2 uv = float2(atan2(v.x, -v.y), asin(-v.z));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float4 main(float3 position : POSITION) : SV_TARGET
{
    float2 uv = SampleSphjericalMap(normalize(position));
    float3 color = envTexture.Sample(linearSampler, uv);
    return float4(color, 1.0);
}
)";

		std::cout << "equirectangularZUP ";

		equirectangularZUP = Shader::fromStr(source, ShaderType::Pixel);
	}

	{
		const std::string source = R"(
cbuffer ProjMatrix : register(b2)
{
	matrix viewProj;
}

static const float3 vertices[36] =
{
            // back face
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            // front face
            -1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            // left face
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            // right face
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            // bottom face
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            // top face
            -1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
};

struct VertexOutput
{
    float3 worldPos : POSITION;
    float4 position : SV_Position;
};

VertexOutput main(uint vertexID : SV_VertexID)
{
    float3 position = vertices[vertexID];
    
    VertexOutput output;
    output.worldPos = position;
    output.position = mul(float4(position,1.0),viewProj);
    
    return output;
}
)";

		std::cout << "equirectangularVShader ";

		equirectangularVShader = Shader::fromStr(source, ShaderType::Vertex);
	}

}

void TextureCube::releaseShader()
{
	delete shader;
	delete equirectangularYUP;
	delete equirectangularZUP;
	delete equirectangularVShader;
}

TextureCube::TextureCube(std::initializer_list<std::string> texturesPath)
{
	Texture2D* textures[6]{};
	{
		int idx = 0;
		for (const std::string& path : texturesPath)
		{
			textures[idx] = new Texture2D(path);
			idx++;
		}
	}

	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = textures[0]->getWidth();
		tDesc.Height = textures[0]->getHeight();
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		tDesc.Format = textures[0]->getFormat();
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tDesc.ArraySize = 6;
		tDesc.MipLevels = 1;
		Renderer::device->CreateTexture2D(&tDesc, nullptr, cubeTexture.ReleaseAndGetAddressOf());
	}

	for (unsigned int i = 0; i < 6; i++)
	{
		RenderAPI::get()->CopySubresourceRegion(cubeTexture.Get(), D3D11CalcSubresource(0, i, 1), 0, 0, 0, textures[i]->getTexture2D(), 0, nullptr);
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textures[0]->getFormat();
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = -1;
		srvDesc.TextureCube.MostDetailedMip = 0;

		createSRV(cubeTexture.Get(), srvDesc);
	}

	for (unsigned int i = 0; i < 6; i++)
	{
		delete textures[i];
	}

}

TextureCube::TextureCube(const std::string& texturePath)
{
	const std::wstring wTexturePath(texturePath.begin(), texturePath.end());
	DirectX::CreateDDSTextureFromFileEx(Renderer::device, Renderer::getContext(), wTexturePath.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DirectX::DDS_LOADER_DEFAULT, nullptr, releaseAndGetSRV());
}

TextureCube::TextureCube(const std::string& texturePath, const UINT& skyboxResolution, const DirectX::XMFLOAT3& up, const unsigned int& mipLevels)
{
	Buffer* buffer = new Buffer(sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	ResourceTexture* equirectangularMap = new ResourceTexture(texturePath);
	Shader* pixelShader = equirectangularZUP;
	
	if (up.y > up.z)
	{
		pixelShader = equirectangularYUP;
	}

	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = skyboxResolution;
	tDesc.Height = skyboxResolution;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tDesc.ArraySize = 6;
	tDesc.MipLevels = mipLevels;
	Renderer::device->CreateTexture2D(&tDesc, nullptr, cubeTexture.ReleaseAndGetAddressOf());

	RenderTargetView* rtv;

	const DirectX::XMFLOAT3 focusPoints[6] =
	{
		DirectX::XMFLOAT3(1.0f,  0.0f,  0.0f),
		DirectX::XMFLOAT3(-1.0f,  0.0f,  0.0f),
		DirectX::XMFLOAT3(0.0f,  1.0f,  0.0f),
		DirectX::XMFLOAT3(0.0f, -1.0f,  0.0f),
		DirectX::XMFLOAT3(0.0f,  0.0f,  1.0f),
		DirectX::XMFLOAT3(0.0f,  0.0f, -1.0f)
	};
	const DirectX::XMFLOAT3 upVectors[6] =
	{
		DirectX::XMFLOAT3(0.0f, 1.0f,  0.0f),
		DirectX::XMFLOAT3(0.0f, 1.0f,  0.0f),
		DirectX::XMFLOAT3(0.0f,  0.0f,  -1.0f),
		DirectX::XMFLOAT3(0.0f,  0.0f, 1.0f),
		DirectX::XMFLOAT3(0.0f, 1.0f,  0.0f),
		DirectX::XMFLOAT3(0.0f, 1.0f,  0.0f)
	};

	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderAPI::get()->PSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);
	RenderAPI::get()->PSSetSRV({ equirectangularMap }, 0);
	RenderAPI::get()->OMSetBlendState(States::get()->defBlendState.Get());
	RenderAPI::get()->RSSetViewport(skyboxResolution, skyboxResolution);
	RenderAPI::get()->VSSetBuffer({ buffer }, 2);

	equirectangularVShader->use();
	pixelShader->use();

	const DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(Math::pi / 2.f, 1.f, 0.1f, 10000.f);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; i++)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, mipLevels);

		rtv = new RenderTargetView(cubeTexture.Get(), rtvDesc);

		RenderAPI::get()->OMSetRTV({ rtv }, nullptr);
		const DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH({ 0.f,0.f,0.f }, { focusPoints[i].x,focusPoints[i].y, focusPoints[i].z }, { upVectors[i].x,upVectors[i].y,upVectors[i].z });
		const DirectX::XMMATRIX viewProj = DirectX::XMMatrixTranspose(viewMatrix * projMatrix);
		memcpy(buffer->map(0).pData, &viewProj, sizeof(DirectX::XMMATRIX));
		buffer->unmap(0);
		RenderAPI::get()->DrawCube();

		delete rtv;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = mipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	createSRV(cubeTexture.Get(), srvDesc);

	if (mipLevels > 1)
	{
		generateMips();
	}

	RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

	delete equirectangularMap;
	delete buffer;
}
