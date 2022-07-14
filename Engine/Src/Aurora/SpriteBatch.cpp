#include<Aurora/SpriteBatch.h>

SpriteBatch* SpriteBatch::instance = nullptr;

int SpriteBatch::TextureSlot::curTextureNum = 0;

SpriteBatch* SpriteBatch::create()
{
	if (instance)
	{
		throw "[class SpriteBatch] already have an instance!";
		return instance;
	}

	instance = new SpriteBatch();

	return instance;
}

SpriteBatch* SpriteBatch::get()
{
	return instance;
}

SpriteBatch::~SpriteBatch()
{
	delete spritePShader;
	delete spriteVShader;
	delete bitmapPShader;
	delete bitmapVShader;
	delete[] texturePool;
}

void SpriteBatch::begin()
{
	fontPool.clear();
}

void SpriteBatch::end()
{
	if (TextureSlot::curTextureNum)
	{
		flush();
	}

	Graphics::context->IASetInputLayout(bitmapInputLayout.Get());

	bitmapPShader->use();
	bitmapVShader->use();

	const unsigned int stride = sizeof(float) * 8;
	const unsigned int offset = 0;

	for (int i = 0; i < fontPool.size(); i++)
	{
		fontPool[i]->updateVerticesData();
		Graphics::context->PSSetShaderResources(0, 1, fontPool[i]->texture2D->resourceView.GetAddressOf());
		Graphics::context->IASetVertexBuffers(0, 1, fontPool[i]->vertexBuffer.GetAddressOf(), &stride, &offset);
		fontPool[i]->render();
	}
}

void SpriteBatch::draw(Texture2D* const texture, const float& x, const float& y)
{
	texturePoolAdd(texture);
	texturePool[texture->poolIndex].addVertices(
		x, y,
		x, y + texture->height,
		x + texture->width, y + texture->height,
		x + texture->width, y
	);
}

void SpriteBatch::draw(Texture2D* const texture, const float& x, const float& y, const float& originX, const float& originY)
{
	texturePoolAdd(texture);
	texturePool[texture->poolIndex].addVertices(
		x - originX, y - originY,
		x - originX, y - originY + texture->height,
		x - originX + texture->width, y - originY + texture->height,
		x - originX + texture->width, y - originY
	);
}

void SpriteBatch::draw(Texture2D* const texture, const float& x, const float& y, const float& originX, const float& originY, const float& rotation)
{
	texturePoolAdd(texture);

	const float cos = cosf(rotation);
	const float sin = sinf(rotation);

	const float left = -originX;
	const float right = -originX + texture->width;
	const float bottom = -originY;
	const float top = -originY + texture->height;

	const float x1 = left * cos - bottom * sin + x;
	const float y1 = left * sin + bottom * cos + y;
	const float x2 = right * cos - bottom * sin + x;
	const float y2 = right * sin + bottom * cos + y;
	const float x3 = right * cos - top * sin + x;
	const float y3 = right * sin + top * cos + y;
	const float x4 = x1 + (x3 - x2);
	const float y4 = y3 - (y2 - y1);;

	texturePool[texture->poolIndex].addVertices(
		x1, y1,
		x4, y4,
		x3, y3,
		x2, y2
	);
}

void SpriteBatch::draw(RenderTexture* const renderTexture, const float& x, const float& y)
{
	draw(renderTexture->getTexture(), x, y);
}

void SpriteBatch::draw(RenderTexture* const renderTexture, const float& x, const float& y, const float& originX, const float& originY)
{
	draw(renderTexture->getTexture(), x, y, originX, originY);
}

void SpriteBatch::draw(RenderTexture* const renderTexture, const float& x, const float& y, const float& originX, const float& originY, const float& rotation)
{
	draw(renderTexture->getTexture(), x, y, originX, originY, rotation);
}

void SpriteBatch::draw(BitmapFont* const font, const std::string& context, const float& x, const float& y, const float& r, const float& g, const float& b, const float& a)
{
	if (!font->idx)
	{
		fontPool.emplace_back(font);
	}

	float currentX = x;
	for (int i = 0; i < context.size(); i++)
	{
		const BitmapFont::Character& character = font->getCharacter(context[i]);
		const float currentY = y + character.yoffset;

		font->vertices[font->idx] = currentX + character.xoffset;
		font->vertices[font->idx + 1] = currentY;
		font->vertices[font->idx + 2] = character.leftTexCoord;
		font->vertices[font->idx + 3] = character.bottomTexCoord;
		font->vertices[font->idx + 4] = r;
		font->vertices[font->idx + 5] = g;
		font->vertices[font->idx + 6] = b;
		font->vertices[font->idx + 7] = a;
		font->vertices[font->idx + 8] = currentX + character.xoffset;
		font->vertices[font->idx + 9] = currentY + character.height * font->scale;
		font->vertices[font->idx + 10] = character.leftTexCoord;
		font->vertices[font->idx + 11] = character.topTexCoord;
		font->vertices[font->idx + 12] = r;
		font->vertices[font->idx + 13] = g;
		font->vertices[font->idx + 14] = b;
		font->vertices[font->idx + 15] = a;
		font->vertices[font->idx + 16] = currentX + character.xoffset + character.width * font->scale;
		font->vertices[font->idx + 17] = currentY + character.height * font->scale;
		font->vertices[font->idx + 18] = character.rightTexCoord;
		font->vertices[font->idx + 19] = character.topTexCoord;
		font->vertices[font->idx + 20] = r;
		font->vertices[font->idx + 21] = g;
		font->vertices[font->idx + 22] = b;
		font->vertices[font->idx + 23] = a;
		font->vertices[font->idx + 24] = currentX + character.xoffset + character.width * font->scale;
		font->vertices[font->idx + 25] = currentY;
		font->vertices[font->idx + 26] = character.rightTexCoord;
		font->vertices[font->idx + 27] = character.bottomTexCoord;
		font->vertices[font->idx + 28] = r;
		font->vertices[font->idx + 29] = g;
		font->vertices[font->idx + 30] = b;
		font->vertices[font->idx + 31] = a;
		font->idx += 32;

		currentX += character.xadvance * font->scale;
	}
}

void SpriteBatch::draw(BitmapFont* const font, const char& context, const float& x, const float& y, const float& r, const float& g, const float& b, const float& a)
{
	if (!font->idx)
	{
		fontPool.emplace_back(font);
	}

	const BitmapFont::Character& character = font->getCharacter(context);

	font->vertices[font->idx] = x + character.xoffset;
	font->vertices[font->idx + 1] = y + character.yoffset;
	font->vertices[font->idx + 2] = character.leftTexCoord;
	font->vertices[font->idx + 3] = character.bottomTexCoord;
	font->vertices[font->idx + 4] = r;
	font->vertices[font->idx + 5] = g;
	font->vertices[font->idx + 6] = b;
	font->vertices[font->idx + 7] = a;
	font->vertices[font->idx + 8] = x + character.xoffset;
	font->vertices[font->idx + 9] = y + character.yoffset + character.height * font->scale;
	font->vertices[font->idx + 10] = character.leftTexCoord;
	font->vertices[font->idx + 11] = character.topTexCoord;
	font->vertices[font->idx + 12] = r;
	font->vertices[font->idx + 13] = g;
	font->vertices[font->idx + 14] = b;
	font->vertices[font->idx + 15] = a;
	font->vertices[font->idx + 16] = x + character.xoffset + character.width * font->scale;
	font->vertices[font->idx + 17] = y + character.yoffset + character.height * font->scale;
	font->vertices[font->idx + 18] = character.rightTexCoord;
	font->vertices[font->idx + 19] = character.topTexCoord;
	font->vertices[font->idx + 20] = r;
	font->vertices[font->idx + 21] = g;
	font->vertices[font->idx + 22] = b;
	font->vertices[font->idx + 23] = a;
	font->vertices[font->idx + 24] = x + character.xoffset + character.width * font->scale;
	font->vertices[font->idx + 25] = y + character.yoffset;
	font->vertices[font->idx + 26] = character.rightTexCoord;
	font->vertices[font->idx + 27] = character.bottomTexCoord;
	font->vertices[font->idx + 28] = r;
	font->vertices[font->idx + 29] = g;
	font->vertices[font->idx + 30] = b;
	font->vertices[font->idx + 31] = a;
	font->idx += 32;
}

void SpriteBatch::compileShaders()
{
	{
		const std::string source = R"(
struct VertexInput
{
    float2 inPos : POSITION;
};

struct VertexOutput
{
    float2 texCoord : TEXCOORD;
    float4 position : SV_Position;
};

cbuffer Matrix2D : register(b0)
{
    matrix proj;
};

static const float2 texCoords[] =
{
    { 0.0, 1.0 },
    { 0.0, 0.0 },
    { 1.0, 0.0 },
    { 1.0, 1.0 }
};

VertexOutput main(VertexInput input, in uint vertexID : SV_VertexID)
{
    VertexOutput output;
    output.texCoord = texCoords[vertexID % 4];
    output.position = mul(float4(input.inPos, 0.0, 1.0), proj);
    return output;
}
		)";

		const ShaderType type = ShaderType::Vertex;

		spriteVShader = Shader::fromStr(source, type);

	}

	{
		const std::string source = R"(
struct PixelInput
{
    float2 texCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
    return objTexture.Sample(objSamplerState, input.texCoord);
}
	)";

		const ShaderType type = ShaderType::Pixel;

		spritePShader = Shader::fromStr(source, type);

	}

	{
		const std::string source = R"(
struct VertexInput
{
    float2 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float4 inColor : COLOR;
};

struct VertexOutput
{
    float2 texCoord : TEXCOORD;
    float4 color : COLOR;
    float4 position : SV_Position;
};

cbuffer Matrix2D : register(b0)
{
    matrix proj;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.texCoord = input.inTexCoord;
    output.color = input.inColor;
    output.position = mul(float4(input.inPos, 0.0, 1.0), proj);
    return output;
}
	)";

		const ShaderType type = ShaderType::Vertex;

		bitmapVShader = Shader::fromStr(source, type);

	}

	{
		const std::string source = R"(
struct PixelInput
{
    float2 texCoord : TEXCOORD;
    float4 color : COLOR;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
    return objTexture.Sample(objSamplerState, input.texCoord) * input.color;
}
	)";

		const ShaderType type = ShaderType::Pixel;

		bitmapPShader = Shader::fromStr(source, type);

	}
}

void SpriteBatch::texturePoolAdd(Texture2D* const texture)
{
	if (texture->poolIndex == -1)
	{
		if (TextureSlot::curTextureNum == TextureSlot::maxTextureSlotNum)
		{
			flush();
		}
		texturePool[TextureSlot::curTextureNum].setTexture(texture);
	}
}

void SpriteBatch::flush()
{
	Graphics::context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	Graphics::context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	Graphics::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	spritePShader->use();
	spriteVShader->use();

	Graphics::context->IASetInputLayout(spriteInputLayout.Get());

	const unsigned int stride = sizeof(float) * 2;
	const unsigned int offset = 0;

	for (int i = 0; i < TextureSlot::curTextureNum; i++)
	{
		texturePool[i].updateVertices();
		Graphics::context->PSSetShaderResources(0, 1, texturePool[i].texture->resourceView.GetAddressOf());
		Graphics::context->IASetVertexBuffers(0, 1, texturePool[i].vertexBuffer.GetAddressOf(), &stride, &offset);
		texturePool[i].drawVertices();
		texturePool[i].reset();
	}

	TextureSlot::curTextureNum = 0;
}

SpriteBatch::SpriteBatch() :
	texturePool(new TextureSlot[TextureSlot::maxTextureSlotNum])
{
	compileShaders();

	//初始化indexBuffer
	{
		unsigned int* indices = new unsigned int[maxSpriteNum * 6];

		for (unsigned int i = 0, j = 0; i < maxSpriteNum * 6; i += 6, j += 4)
		{
			indices[i] = j;
			indices[i + 1] = j + 1;
			indices[i + 2] = j + 2;
			indices[i + 3] = j + 2;
			indices[i + 4] = j + 3;
			indices[i + 5] = j;
		}

		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * 6 * maxSpriteNum;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		Graphics::device->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer.ReleaseAndGetAddressOf());

		delete[] indices;
	}

	//初始化spriteInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout =
		{
				"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
		};

		Graphics::device->CreateInputLayout(&layout, 1u,
			spriteVShader->shaderBlob->GetBufferPointer(),
			spriteVShader->shaderBlob->GetBufferSize(),
			spriteInputLayout.ReleaseAndGetAddressOf());
	}

	//初始化bitmapInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout[3] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		Graphics::device->CreateInputLayout(layout, 3u,
			bitmapVShader->shaderBlob->GetBufferPointer(),
			bitmapVShader->shaderBlob->GetBufferSize(),
			bitmapInputLayout.ReleaseAndGetAddressOf());
	}

	//初始化samplerState
	{
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		Graphics::device->CreateSamplerState(&sampDesc, samplerState.ReleaseAndGetAddressOf());
	}
}

SpriteBatch::TextureSlot::TextureSlot() :
	texture(nullptr), vertices(new float[maxSpriteNum * 8]), idx(0)
{
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(float) * maxSpriteNum * 8;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	Graphics::device->CreateBuffer(&vertexBufferDesc, nullptr, vertexBuffer.ReleaseAndGetAddressOf());
}

SpriteBatch::TextureSlot::~TextureSlot()
{
	delete[] vertices;
}

void SpriteBatch::TextureSlot::setTexture(Texture2D* const texture)
{
	this->texture = texture;
	texture->poolIndex = curTextureNum++;
}

void SpriteBatch::TextureSlot::updateVertices() const
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Graphics::context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, vertices, sizeof(float) * idx);
	Graphics::context->Unmap(vertexBuffer.Get(), 0);
}

void SpriteBatch::TextureSlot::drawVertices() const
{
	Graphics::context->DrawIndexed(idx / 4 * 3, 0, 0);
}

void SpriteBatch::TextureSlot::addVertices(const float& x1, const float& y1, const float& x2, const float& y2, const float& x3, const float& y3, const float& x4, const float& y4)
{
	vertices[idx] = x1;
	vertices[idx + 1] = y1;
	vertices[idx + 2] = x2;
	vertices[idx + 3] = y2;
	vertices[idx + 4] = x3;
	vertices[idx + 5] = y3;
	vertices[idx + 6] = x4;
	vertices[idx + 7] = y4;
	idx += 8;
}

void SpriteBatch::TextureSlot::reset()
{
	texture->poolIndex = -1;
	idx = 0;
}
