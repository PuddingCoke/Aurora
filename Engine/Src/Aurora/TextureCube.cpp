#include<Aurora/TextureCube.h>

Shader* TextureCube::shader;

TextureCube* TextureCube::create(std::initializer_list<std::string> texturesPath)
{
	return new TextureCube(texturesPath);
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

void TextureCube::releaseShader()
{
    delete shader;
}

TextureCube::TextureCube(std::initializer_list<std::string> texturesPath)
{
	Texture2D* textures[6];
	{
		int idx = 0;
		for (const std::string& path : texturesPath)
		{
			textures[idx] = Texture2D::create(path, D3D11_USAGE_STAGING, (D3D11_BIND_FLAG)0, D3D11_CPU_ACCESS_READ);
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
		D3D11_MAPPED_SUBRESOURCE mappedData;
		Renderer::context->Map(textures[i]->getTexture2D(), 0, D3D11_MAP_READ, 0, &mappedData);
		Renderer::context->UpdateSubresource(cubeTexture.Get(), D3D11CalcSubresource(0, i, 1), 0,
			mappedData.pData, mappedData.RowPitch, mappedData.DepthPitch);
		Renderer::context->Unmap(textures[i]->getTexture2D(), 0);
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
