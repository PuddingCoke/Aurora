#include<Aurora/Core/PrimitiveBatch.h>

PrimitiveBatch* PrimitiveBatch::instance = nullptr;

PrimitiveBatch::PrimitiveBatch() :
	lineBuffer(new ConstantBuffer(sizeof(LineParam), D3D11_USAGE_DYNAMIC))
{
	compileShaders();

	//初始化primitiveInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout[2] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		Renderer::getDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(lineVS), primitiveInputLayout.ReleaseAndGetAddressOf());
	}

	//初始化circleInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout[3] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"POSITION",1,DXGI_FORMAT_R32_FLOAT,0,8,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D11_INPUT_PER_INSTANCE_DATA,1}
		};

		Renderer::getDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(circleVS), circleInputLayout.ReleaseAndGetAddressOf());
	}

	//初始化rcLineInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout[3] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"POSITION", 1, DXGI_FORMAT_R32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		Renderer::getDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(rcLineVS), rcLineInputLayout.ReleaseAndGetAddressOf());
	}

	setLineWidth(1.f);
	applyChange();
}

void PrimitiveBatch::compileShaders()
{
	lineVS = new Shader(g_PrimitiveBatchLineVSBytes, sizeof(g_PrimitiveBatchLineVSBytes), ShaderType::Vertex);
	circleVS = new Shader(g_PrimitiveBatchCircleVSBytes, sizeof(g_PrimitiveBatchCircleVSBytes), ShaderType::Vertex);
	rcLineVS = new Shader(g_PrimitiveBatchRCLineVSBytes, sizeof(g_PrimitiveBatchRCLineVSBytes), ShaderType::Vertex);

	primitivePS = new Shader(g_PrimitiveBatchPSBytes, sizeof(g_PrimitiveBatchPSBytes), ShaderType::Pixel);

	lineGS = new Shader(g_PrimitiveBatchLineGSBytes, sizeof(g_PrimitiveBatchLineGSBytes), ShaderType::Geometry);
	rcLineGS = new Shader(g_PrimitiveBatchRCLineGSBytes, sizeof(g_PrimitiveBatchRCLineGSBytes), ShaderType::Geometry);
}

PrimitiveBatch* PrimitiveBatch::create()
{
	if (instance)
	{
		throw "[class PrimitiveBatch] Already have an instance!";
	}

	instance = new PrimitiveBatch();

	return instance;
}

PrimitiveBatch* PrimitiveBatch::get()
{
	return instance;
}

PrimitiveBatch::~PrimitiveBatch()
{
	delete lineVS;
	delete circleVS;
	delete rcLineVS;
	delete primitivePS;
	delete lineGS;
	delete rcLineGS;
	delete lineBuffer;
}

void PrimitiveBatch::begin()
{
	lineRenderer.begin();
	circleRenderer.begin();
	rcLineRenderer.begin();
}

void PrimitiveBatch::end()
{
	RenderAPI::get()->GSSetConstantBuffer({ lineBuffer }, 2);

	RenderAPI::get()->IASetInputLayout(primitiveInputLayout.Get());

	lineVS->use();
	primitivePS->use();
	lineGS->use();

	lineRenderer.end();

	RenderAPI::get()->IASetInputLayout(circleInputLayout.Get());
	circleVS->use();

	circleRenderer.end();

	RenderAPI::get()->IASetInputLayout(rcLineInputLayout.Get());
	rcLineVS->use();
	rcLineGS->use();

	rcLineRenderer.end();

	RenderAPI::get()->GSUnbindShader();
}

void PrimitiveBatch::drawLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& r, const float& g, const float& b, const float& a)
{
	lineRenderer.addLine(x1, y1, x2, y2, r, g, b, a);
}

void PrimitiveBatch::drawCircle(const float& x, const float& y, const float& length, const float& r, const float& g, const float& b, const float& a)
{
	circleRenderer.addCircle(x, y, length, r, g, b, a);
}

void PrimitiveBatch::drawRoundCapLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& width, const float& r, const float& g, const float& b, const float& a)
{
	rcLineRenderer.addRoundCapLine(x1, y1, x2, y2, width, r, g, b, a);
}

void PrimitiveBatch::setLineWidth(const float& width)
{
	lineParam.lineWidth = width / 2.f;
}

void PrimitiveBatch::applyChange() const
{
	memcpy(lineBuffer->map().pData, &lineParam, sizeof(LineParam));
	lineBuffer->unmap();
}

PrimitiveBatch::LineRenderer::LineRenderer() :
	vertices(new float[2 * 6 * maxLineNum]), idx(0),
	vertexBuffer(new VertexBuffer(sizeof(float) * 2 * 6 * maxLineNum, D3D11_USAGE_DYNAMIC, nullptr))
{
}

PrimitiveBatch::LineRenderer::~LineRenderer()
{
	delete vertexBuffer;
	delete[] vertices;
}

void PrimitiveBatch::LineRenderer::begin()
{
	idx = 0;
}

void PrimitiveBatch::LineRenderer::end()
{
	updateVerticesData();

	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	RenderAPI::get()->IASetVertexBuffer(0, { vertexBuffer }, { sizeof(float) * 6 }, { 0 });

	RenderAPI::get()->Draw(idx / 6, 0);
}

void PrimitiveBatch::LineRenderer::updateVerticesData() const
{
	memcpy(vertexBuffer->map().pData, vertices, sizeof(float) * idx);
	vertexBuffer->unmap();
}

void PrimitiveBatch::LineRenderer::addLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& r, const float& g, const float& b, const float& a)
{
	vertices[idx] = x1;
	vertices[idx + 1] = y1;
	vertices[idx + 2] = r;
	vertices[idx + 3] = g;
	vertices[idx + 4] = b;
	vertices[idx + 5] = a;
	vertices[idx + 6] = x2;
	vertices[idx + 7] = y2;
	vertices[idx + 8] = r;
	vertices[idx + 9] = g;
	vertices[idx + 10] = b;
	vertices[idx + 11] = a;
	idx += 12;
}

PrimitiveBatch::CircleRenderer::CircleRenderer() :
	vertices(new float[maxCircleNum * 7]), idx(0),
	vertexBuffer(new VertexBuffer(sizeof(float) * 7 * maxCircleNum, D3D11_USAGE_DYNAMIC, nullptr))
{
}

PrimitiveBatch::CircleRenderer::~CircleRenderer()
{
	delete vertexBuffer;
	delete[] vertices;
}

void PrimitiveBatch::CircleRenderer::begin()
{
	idx = 0;
}

void PrimitiveBatch::CircleRenderer::end()
{
	updateVerticesData();

	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	RenderAPI::get()->IASetVertexBuffer(0, { vertexBuffer }, { sizeof(float) * 7u }, { 0 });

	RenderAPI::get()->DrawInstanced(128, idx / 7, 0, 0);
}

void PrimitiveBatch::CircleRenderer::updateVerticesData() const
{
	memcpy(vertexBuffer->map().pData, vertices, sizeof(float) * idx);
	vertexBuffer->unmap();
}

void PrimitiveBatch::CircleRenderer::addCircle(const float& x, const float& y, const float& length, const float& r, const float& g, const float& b, const float& a)
{
	if (length < 1.f)
	{
		return;
	}
	vertices[idx] = x;
	vertices[idx + 1] = y;
	vertices[idx + 2] = length;
	vertices[idx + 3] = r;
	vertices[idx + 4] = g;
	vertices[idx + 5] = b;
	vertices[idx + 6] = a;
	idx += 7;
}

PrimitiveBatch::RCLineRenderer::RCLineRenderer() :
	vertices(new float[maxLineNum * 7 * 2]), idx(0),
	vertexBuffer(new VertexBuffer(sizeof(float) * 2 * 7 * maxLineNum, D3D11_USAGE_DYNAMIC, nullptr))
{
}

PrimitiveBatch::RCLineRenderer::~RCLineRenderer()
{
	delete vertexBuffer;
	delete[] vertices;
}

void PrimitiveBatch::RCLineRenderer::begin()
{
	idx = 0;
}

void PrimitiveBatch::RCLineRenderer::end()
{
	updateVerticesData();

	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	RenderAPI::get()->IASetVertexBuffer(0, { vertexBuffer }, { sizeof(float) * 7 }, { 0 });

	RenderAPI::get()->Draw(idx / 7, 0);
}

void PrimitiveBatch::RCLineRenderer::updateVerticesData() const
{
	memcpy(vertexBuffer->map().pData, vertices, sizeof(float) * idx);
	vertexBuffer->unmap();
}

void PrimitiveBatch::RCLineRenderer::addRoundCapLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& width, const float& r, const float& g, const float& b, const float& a)
{
	vertices[idx] = x1;
	vertices[idx + 1] = y1;
	vertices[idx + 2] = width / 2.f;
	vertices[idx + 3] = r;
	vertices[idx + 4] = g;
	vertices[idx + 5] = b;
	vertices[idx + 6] = a;
	vertices[idx + 7] = x2;
	vertices[idx + 8] = y2;
	vertices[idx + 9] = width / 2.f;
	vertices[idx + 10] = r;
	vertices[idx + 11] = g;
	vertices[idx + 12] = b;
	vertices[idx + 13] = a;

	idx += 14;
}
