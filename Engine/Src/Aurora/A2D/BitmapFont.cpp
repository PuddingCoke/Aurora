#include<Aurora/A2D/BitmapFont.h>

BitmapFont* BitmapFont::create(const std::wstring& bitmapPath, const std::string& configFilePath, const int& fontSize)
{
	return new BitmapFont(bitmapPath, configFilePath, fontSize);
}

BitmapFont::~BitmapFont()
{
	delete texture2D;
	delete[] vertices;
}

const BitmapFont::Character& BitmapFont::getCharacter(const char& c)
{
	return characterMap[c];
}

const int& BitmapFont::getFontSize() const
{
	return fontSize;
}

void BitmapFont::setFontSize(const int& fontSize)
{
	this->fontSize = fontSize;
	scale = (float)fontSize / originFontSize;
}

const float& BitmapFont::getScale() const
{
	return scale;
}

BitmapFont::BitmapFont(const std::wstring& bitmapPath, const std::string& configFilePath, const int& fontSize) :
	fontSize(fontSize), originFontSize(0), scale(1), idx(0), vertices(new float[maxCharacterCount * 32]), texture2D(Texture2D::create(bitmapPath))
{
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
			if (fontSize < 0)
			{
				this->fontSize = originFontSize;
			}
			scale = (float)this->fontSize / originFontSize;
		}
	}

	stream >> temp;

	Parse(temp);
	const int count = std::stoi(value);

	int bitmapWidth = texture2D->getWidth(), bitmapHeight = texture2D->getHeight();

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

		/*
		int xoffset;
		int yoffset;
		int xadvance;

		float width;
		float height;

		float leftTexCoord;
		float rightTexCoord;
		float bottomTexCoord;
		float topTexCoord;
		*/
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

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(float) * maxCharacterCount * 32;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	Graphics::device->CreateBuffer(&vertexBufferDesc, nullptr, vertexBuffer.ReleaseAndGetAddressOf());
}

void BitmapFont::updateVerticesData() const
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Graphics::context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, vertices, sizeof(float) * idx);
	Graphics::context->Unmap(vertexBuffer.Get(), 0);
}

void BitmapFont::render()
{
	Graphics::context->DrawIndexed(idx / 16 * 3, 0, 0);
	idx = 0;
}
