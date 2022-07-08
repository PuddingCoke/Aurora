#include<Aurora/Shader.h>

Shader* Shader::displayVShader;
Shader* Shader::displayPShader;

void Shader::use()
{
	useFunc();
}

Shader* Shader::fromFile(const std::string& filePath, const ShaderType& type)
{
	std::cout << "[class Shader] " << filePath << " ";
	std::string source = Utils::File::readAllText(filePath);
	return new Shader(source, type);
}

Shader* Shader::fromStr(const std::string& source, const ShaderType& type)
{
	std::cout << "[class Shader] Raw string ";
	return new Shader(source, type);
}

void Shader::useDisplayShader()
{
	Shader::displayPShader->use();
	Shader::displayVShader->use();
	Graphics::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());
}

void Shader::ini()
{
	//初始化displayVShader
	{
		const std::string source=R"(
static const float2 positions[] =
{
    { -1.0, 1.0 },
    { 3.0, 1.0 },
    { -1.0, -3.0 }
};

static const float2 texCoords[] =
{
    { 0.0, 0.0 },
    { 2.0, 0.0 },
    { 0.0, 2.0 }
};

struct VertexOutput
{
    float2 texCoord : TEXCOORD;
    float4 position : SV_Position;
};

VertexOutput main(uint vertexID : SV_VertexID)
{
    VertexOutput output;
    output.texCoord = texCoords[vertexID];
    output.position = float4(positions[vertexID], 0.0, 1.0);
    return output;
}
		)";

		const ShaderType type = ShaderType::Vertex;

		displayVShader = Shader::fromStr(source, type);
	}

	//初始化displayPShader
	{
		const std::string source = R"(
Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(float2 texCoord:TEXCOORD) : SV_TARGET
{
    return objTexture.Sample(objSamplerState, texCoord);
}
		)";

		const ShaderType type = ShaderType::Pixel;

		displayPShader = Shader::fromStr(source, type);
	}
}

void Shader::release()
{
	delete displayVShader;
	delete displayPShader;
}

Shader::Shader(const std::string& source, const ShaderType& type)
{
	HRESULT hr;

	switch (type)
	{
	case ShaderType::Compute:
	{
		hr = D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr, "main", "cs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		ID3D11ComputeShader* cs = nullptr;
		Graphics::device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &cs);
		shaderPtr = cs;
		useFunc = [this]()
		{
			Graphics::context->CSSetShader((ID3D11ComputeShader*)shaderPtr.Get(), nullptr, 0);
		};
	}
	break;
	case ShaderType::Domain:
	{
		hr = D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr, "main", "ds_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		ID3D11DomainShader* ds = nullptr;
		Graphics::device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ds);
		shaderPtr = ds;
		useFunc = [this]()
		{
			Graphics::context->DSSetShader((ID3D11DomainShader*)shaderPtr.Get(), nullptr, 0);
		};
	}
	break;
	case ShaderType::Geometry:
	{
		hr = D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr, "main", "gs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		ID3D11GeometryShader* gs = nullptr;
		Graphics::device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &gs);
		shaderPtr = gs;
		useFunc = [this]()
		{
			Graphics::context->GSSetShader((ID3D11GeometryShader*)shaderPtr.Get(), nullptr, 0);
		};
	}
	break;
	case ShaderType::Hull:
	{
		hr = D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr, "main", "hs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		ID3D11HullShader* hs = nullptr;
		Graphics::device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &hs);
		shaderPtr = hs;
		useFunc = [this]()
		{
			Graphics::context->HSSetShader((ID3D11HullShader*)shaderPtr.Get(), nullptr, 0);
		};
	}
	break;
	case ShaderType::Pixel:
	{
		hr = D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr, "main", "ps_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		ID3D11PixelShader* ps = nullptr;
		Graphics::device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &ps);
		shaderPtr = ps;
		useFunc = [this]()
		{
			Graphics::context->PSSetShader((ID3D11PixelShader*)shaderPtr.Get(), nullptr, 0);
		};
	}
	break;
	case ShaderType::Vertex:
	{
		hr = D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, shaderBlob.ReleaseAndGetAddressOf(), nullptr);
		ID3D11VertexShader* vs = nullptr;
		Graphics::device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &vs);
		shaderPtr = vs;
		useFunc = [this]()
		{
			Graphics::context->VSSetShader((ID3D11VertexShader*)shaderPtr.Get(), nullptr, 0);
		};
	}
	break;
	default:
		hr = S_OK;
		break;
	}

	if (SUCCEEDED(hr))
	{
		std::wcout << "compile successfully!\n";
	}
	else
	{
		std::wcout << "compile failed!\n";
		return;
	}
}
