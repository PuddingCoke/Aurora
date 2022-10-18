#include<Aurora/A3D/TextureCube.h>

Shader* TextureCube::shader;

Shader* TextureCube::equirectangularYUP;

Shader* TextureCube::equirectangularZUP;

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

void TextureCube::setSRV(const UINT& slot)
{
	Renderer::context->PSSetShaderResources(slot, 1, cubeSRV.GetAddressOf());
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

}

void TextureCube::releaseShader()
{
	delete shader;
	delete equirectangularYUP;
	delete equirectangularZUP;
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
		Renderer::context->CopySubresourceRegion(cubeTexture.Get(), D3D11CalcSubresource(0, i, 1), 0, 0, 0, textures[i]->getTexture2D(), 0, nullptr);
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textures[0]->getFormat();
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = -1;
		srvDesc.TextureCube.MostDetailedMip = 0;

		Renderer::device->CreateShaderResourceView(cubeTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());
	}

	for (unsigned int i = 0; i < 6; i++)
	{
		delete textures[i];
	}

}

TextureCube::TextureCube(const std::string& texturePath)
{
	const std::wstring wTexturePath(texturePath.begin(), texturePath.end());
	DirectX::CreateDDSTextureFromFileEx(Renderer::device, Renderer::context, wTexturePath.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DirectX::DDS_LOADER_DEFAULT, nullptr, cubeSRV.ReleaseAndGetAddressOf());
}

TextureCube::TextureCube(const std::string& texturePath, const UINT& skyboxResolution, const DirectX::XMFLOAT3& up, const unsigned int& mipLevels)
{
	ResourceTexture* equirectangularMap = new ResourceTexture(texturePath);
	Shader* pixelShader = equirectangularZUP;
	if (up.y > up.z)
	{
		pixelShader = equirectangularYUP;
	}

	RenderTexture* renderTexture = new RenderTexture(skyboxResolution, skyboxResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Transparent);

	if (mipLevels > 1)
	{
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
	}
	else
	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = skyboxResolution;
		tDesc.Height = skyboxResolution;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tDesc.ArraySize = 6;
		tDesc.MipLevels = 1;
		Renderer::device->CreateTexture2D(&tDesc, nullptr, cubeTexture.ReleaseAndGetAddressOf());
	}

	const DirectX::XMFLOAT3 eye = { 0,0,0 };
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

	RenderAPI::get()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Renderer::context->PSSetSamplers(0, 1, States::get()->linearClampSampler.GetAddressOf());

	RenderAPI::get()->PSSetSRV({ equirectangularMap }, 0);

	TextureCube::shader->use();
	pixelShader->use();

	RenderAPI::get()->SetBlendState(States::get()->defBlendState.Get());

	RenderAPI::get()->OMSetRTV({ renderTexture }, nullptr);

	RenderAPI::get()->SetViewport(skyboxResolution, skyboxResolution);

	Camera::setProj(Math::pi / 2.f, 1.f, 0.1f, 1000.f);

	for (int i = 0; i < 6; i++)
	{
		renderTexture->clearRTV(DirectX::Colors::Black);
		Camera::setView(eye, focusPoints[i], upVectors[i]);
		Renderer::context->Draw(36, 0);
		Renderer::context->CopySubresourceRegion(cubeTexture.Get(), D3D11CalcSubresource(0, i, mipLevels), 0, 0, 0, renderTexture->getTexture2D(), 0, nullptr);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = mipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	Renderer::device->CreateShaderResourceView(cubeTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());

	if (mipLevels > 1)
	{
		Renderer::context->GenerateMips(cubeSRV.Get());
	}

	RenderAPI::get()->SetViewport(Graphics::getWidth(), Graphics::getHeight());

	delete renderTexture;
	delete equirectangularMap;
}
