#include<Aurora/A2D/SpriteBatch.h>

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
	delete indexBuffer;
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

	RenderAPI::get()->IASetInputLayout(bitmapInputLayout.Get());

	bitmapPShader->use();
	bitmapVShader->use();

	RenderAPI::get()->PSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);
	RenderAPI::get()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < fontPool.size(); i++)
	{
		fontPool[i]->updateVerticesData();
		RenderAPI::get()->PSSetSRV({ fontPool[i]->rTexture }, 0);
		RenderAPI::get()->IASetVertexBuffer(0, { fontPool[i]->vertexBuffer }, { sizeof(float) * 8 }, { 0 });
		fontPool[i]->render();
	}
}

void SpriteBatch::draw(Texture2D* const texture, ShaderResourceView* const srv, const float& x, const float& y)
{
	int index = texturePoolAdd(srv);
	texturePool[index].addVertices(
		x, y,
		x, y + texture->getHeight(),
		x + texture->getWidth(), y + texture->getHeight(),
		x + texture->getWidth(), y
	);
}

void SpriteBatch::draw(Texture2D* const texture, ShaderResourceView* const srv, const float& x, const float& y, const float& originX, const float& originY)
{
	int index = texturePoolAdd(srv);
	texturePool[index].addVertices(
		x - originX, y - originY,
		x - originX, y - originY + texture->getHeight(),
		x - originX + texture->getWidth(), y - originY + texture->getHeight(),
		x - originX + texture->getWidth(), y - originY
	);
}

void SpriteBatch::draw(Texture2D* const texture, ShaderResourceView* const srv, const float& x, const float& y, const float& originX, const float& originY, const float& rotation)
{
	int index = texturePoolAdd(srv);

	const float cos = cosf(rotation);
	const float sin = sinf(rotation);

	const float left = -originX;
	const float right = -originX + texture->getWidth();
	const float bottom = -originY;
	const float top = -originY + texture->getHeight();

	const float x1 = left * cos - bottom * sin + x;
	const float y1 = left * sin + bottom * cos + y;
	const float x2 = right * cos - bottom * sin + x;
	const float y2 = right * sin + bottom * cos + y;
	const float x3 = right * cos - top * sin + x;
	const float y3 = right * sin + top * cos + y;
	const float x4 = x1 + (x3 - x2);
	const float y4 = y3 - (y2 - y1);;

	texturePool[index].addVertices(
		x1, y1,
		x4, y4,
		x3, y3,
		x2, y2
	);
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
	spriteVShader = new Shader(g_SpriteBatchSpriteVSBytes, sizeof(g_SpriteBatchSpriteVSBytes), ShaderType::Vertex);
	spritePShader = new Shader(g_SpriteBatchSpritePSBytes, sizeof(g_SpriteBatchSpritePSBytes), ShaderType::Pixel);

	bitmapVShader = new Shader(g_SpriteBatchTextVSBytes, sizeof(g_SpriteBatchTextVSBytes), ShaderType::Vertex);
	bitmapPShader = new Shader(g_SpriteBatchTextPSBytes, sizeof(g_SpriteBatchTextPSBytes), ShaderType::Pixel);
}

int SpriteBatch::texturePoolAdd(ShaderResourceView* const texture)
{
	for (int i = 0; i < TextureSlot::curTextureNum; i++)
	{
		if (texturePool[i].textureSRV == texture)
		{
			return i;
		}
	}

	if (TextureSlot::curTextureNum == TextureSlot::maxTextureSlotNum)
	{
		flush();
	}

	texturePool[TextureSlot::curTextureNum].textureSRV = texture;

	return TextureSlot::curTextureNum++;
}

void SpriteBatch::flush()
{
	RenderAPI::get()->PSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);
	RenderAPI::get()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	spritePShader->use();
	spriteVShader->use();

	RenderAPI::get()->IASetInputLayout(spriteInputLayout.Get());

	for (int i = 0; i < TextureSlot::curTextureNum; i++)
	{
		texturePool[i].updateVertices();
		RenderAPI::get()->PSSetSRV({ texturePool[i].textureSRV }, 0);
		RenderAPI::get()->IASetVertexBuffer(0, { texturePool[i].vertexBuffer }, { sizeof(float) * 2 }, { 0 });
		texturePool[i].drawVertices();
		texturePool[i].idx = 0;
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

		indexBuffer = new Buffer(sizeof(unsigned int) * 6 * maxSpriteNum, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE, indices);

		delete[] indices;
	}

	//初始化spriteInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout =
		{
				"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0
		};

		Renderer::device->CreateInputLayout(&layout, 1u, SHADERDATA(spriteVShader), spriteInputLayout.ReleaseAndGetAddressOf());
	}

	//初始化bitmapInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout[3] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(bitmapVShader), bitmapInputLayout.ReleaseAndGetAddressOf());
	}
}

SpriteBatch::TextureSlot::TextureSlot() :
	textureSRV(nullptr), vertices(new float[maxSpriteNum * 8]), idx(0),
	vertexBuffer(new Buffer(sizeof(float)* maxSpriteNum * 8, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE))
{
}

SpriteBatch::TextureSlot::~TextureSlot()
{
	delete vertexBuffer;
	delete[] vertices;
}

void SpriteBatch::TextureSlot::updateVertices() const
{
	memcpy(vertexBuffer->map(0).pData, vertices, sizeof(float) * idx);
	vertexBuffer->unmap(0);
}

void SpriteBatch::TextureSlot::drawVertices() const
{
	RenderAPI::get()->DrawIndexed(idx / 4 * 3, 0, 0);
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
