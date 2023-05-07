#pragma once

#include<unordered_map>

#include<Aurora/Core/RenderAPI.h>
#include<Aurora/Core/Resource/ResourceTexture.h>

class TextBatch
{
public:

	Shader* spriteVShader;

	Shader* spriteGShader;

	Shader* spritePShader;

	float fontSize;

	float originFontSize;

	float scale;

	ResourceTexture* rTexture;

	size_t idx;

	static constexpr size_t maxTextNum = 32768;

	Buffer* textBuffer;

	struct Text
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 size;
		DirectX::XMFLOAT4 color;
		float uvLeft;
		float uvRight;
		float uvBottom;
		float uvTop;
	} *textArray;

	struct Character
	{
		int xoffset;
		int yoffset;
		int xadvance;
		float width;
		float height;
		float leftTexCoord;
		float rightTexCoord;
		float bottomTexCoord;
		float topTexCoord;
	};

	std::unordered_map<char, const Character> characterMap;

	ComPtr<ID3D11InputLayout> inputLayout;

	TextBatch(const std::string& bitmapPath, const std::string& configFilePath, const float& fontSize) :
		fontSize(fontSize), originFontSize(0), scale(1), idx(0),
		rTexture(new ResourceTexture(bitmapPath)),
		spriteVShader(new Shader("SpriteVShader.hlsl", ShaderType::Vertex)),
		spriteGShader(new Shader("SpriteGShader.hlsl", ShaderType::Geometry)),
		spritePShader(new Shader("SpritePShader.hlsl", ShaderType::Pixel)),
		textBuffer(new Buffer(sizeof(Text)* maxTextNum, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		textArray(new Text[maxTextNum])
	{
		D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",1,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",2,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",3,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		Renderer::getDevice()->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), SHADERDATA(spriteVShader), inputLayout.ReleaseAndGetAddressOf());

		std::ifstream stream(configFilePath);

		std::string temp;
		std::string attr;
		std::string value;

		auto Parse = [&](const std::string& str)->void
		{
			for (int i = 0; i < str.size(); i++)
			{
				if (str[i] == '=')
				{
					attr = str.substr(0, i);
					value = str.substr(i + 1, str.size() - i - 1);
					break;
				}
			}
		};

		while (1)
		{
			stream >> temp;
			if (temp == "chars")
			{
				break;
			}
			else if (temp.find("size") != std::string::npos)
			{
				Parse(temp);
				originFontSize = std::stoi(value);
				scale = this->fontSize / originFontSize;
			}
		}

		stream >> temp;

		Parse(temp);
		const int count = std::stoi(value);

		int bitmapWidth = rTexture->getWidth(), bitmapHeight = rTexture->getHeight();

		for (int i = 0; i < count; i++)
		{
			int charID, startX, startY, width, height, xoffset, yoffset, xadvance;

			stream >> temp;

			for (int attrIndex = 0; attrIndex < 10; attrIndex++)
			{
				stream >> temp;

				Parse(temp);

				if (attr == "id")
				{
					charID = std::stoi(value);
				}
				else if (attr == "x")
				{
					startX = std::stoi(value);
				}
				else if (attr == "y")
				{
					startY = std::stoi(value);
				}
				else if (attr == "width")
				{
					width = std::stoi(value);
				}
				else if (attr == "height")
				{
					height = std::stoi(value);
				}
				else if (attr == "xoffset")
				{
					xoffset = std::stoi(value);
				}
				else if (attr == "yoffset")
				{
					yoffset = std::stoi(value);
				}
				else if (attr == "xadvance")
				{
					xadvance = std::stoi(value);
				}
			}

			Character character{
				xoffset,
				-yoffset - height,
				xadvance,
				(float)width,
				(float)height,
				startX / (float)bitmapWidth,
				(startX + width) / (float)bitmapWidth,
				(startY + height) / (float)(bitmapHeight),
				startY / (float)bitmapHeight
			};

			characterMap.insert(std::pair<char, const Character>(charID, character));

		}

		stream.close();
	}

	void drawText(const char& context, const float& x, const float& y, const float& z, const float& r, const float& g, const float& b, const float& a)
	{
		const Character& character = characterMap[context];
		textArray[idx].position = DirectX::XMFLOAT3(x, y, z); 
		textArray[idx].size = DirectX::XMFLOAT2(character.width * scale, character.height * scale);
		textArray[idx].color = DirectX::XMFLOAT4(r, g, b, a);
		textArray[idx].uvLeft = character.leftTexCoord;
		textArray[idx].uvRight = character.rightTexCoord;
		textArray[idx].uvBottom = character.bottomTexCoord;
		textArray[idx].uvTop = character.topTexCoord;
		idx++;
	}

	void render()
	{
		memcpy(textBuffer->map().pData, textArray, sizeof(Text) * idx);
		textBuffer->unmap();

		spriteVShader->use();
		spriteGShader->use();
		spritePShader->use();

		RenderAPI::get()->PSSetSRV({ rTexture }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		RenderAPI::get()->IASetInputLayout(inputLayout.Get());
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		RenderAPI::get()->IASetVertexBuffer(0, { textBuffer }, { sizeof(Text) }, { 0 });

		RenderAPI::get()->Draw(idx, 0);
		idx = 0;

		RenderAPI::get()->VSSetShader(nullptr);
		RenderAPI::get()->GSSetShader(nullptr);
		RenderAPI::get()->PSSetShader(nullptr);
	}

	~TextBatch()
	{
		delete spriteVShader;
		delete spriteGShader;
		delete spritePShader;
		delete textBuffer;
		delete textArray;
		delete rTexture;
	}

};