#include<Aurora/A2D/PrimitiveBatch.h>

PrimitiveBatch* PrimitiveBatch::instance = nullptr;

PrimitiveBatch::PrimitiveBatch()
{
	compileShaders();

	//初始化primitiveInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout[2] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(primitive2DVShader), primitiveInputLayout.ReleaseAndGetAddressOf());
	}

	//初始化circleInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout[4] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"POSITION",1,DXGI_FORMAT_R32G32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"POSITION",2,DXGI_FORMAT_R32_FLOAT,1,8,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,12,D3D11_INPUT_PER_INSTANCE_DATA,1}
		};

		Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(circleVShader), circleInputLayout.ReleaseAndGetAddressOf());
	}

	//初始化rcLineInputLayout
	{
		D3D11_INPUT_ELEMENT_DESC layout[3] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"POSITION", 1, DXGI_FORMAT_R32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(rcLineVShader), rcLineInputLayout.ReleaseAndGetAddressOf());
	}

	//初始化lineBuffer
	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(LineParam);
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Renderer::device->CreateBuffer(&bd, nullptr, lineBuffer.ReleaseAndGetAddressOf());
	}

	setLineWidth(1.f);
	applyChange();
}

void PrimitiveBatch::compileShaders()
{
	{
		const std::string source = R"(
struct VertexInput
{
    float2 inPos : POSITION;
    float4 inColor : COLOR;
};

struct VertexOutput
{
	float4 position : SV_Position;
    float4 color : COLOR;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.color = input.inColor;
    output.position = float4(input.inPos, 0.0, 1.0);
    return output;
}
	)";

		const ShaderType type = ShaderType::Vertex;

		primitive2DVShader = Shader::fromStr(source, type);

	}

	{
		const std::string source = R"(
struct VertexInput
{
    float2 inPos : POSITION0;
    float2 circlePos : POSITION1;
    float circleLength : POSITION2;
    float4 circleColor : COLOR;
};

struct VertexOutput
{
	float4 position : SV_Position;
    float4 color : COLOR;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.color = input.circleColor;
    output.position = float4(input.circleLength * input.inPos + input.circlePos, 0.0, 1.0);
    return output;
}
	)";

		const ShaderType type = ShaderType::Vertex;

		circleVShader = Shader::fromStr(source, type);

	}

	{
		const std::string source = R"(
struct VertexInput
{
    float2 inPos : POSITION0;
    float inWidth : POSITION1;
    float4 inColor : COLOR;
};

struct VertexOutput
{
    float4 pos : SV_Position;
    float width : POSITION1;
    float4 color : COLOR;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.pos = float4(input.inPos, 0.0, 1.0);
    output.width = input.inWidth;
    output.color = input.inColor;
    return output;
}
	)";

		const ShaderType type = ShaderType::Vertex;

		rcLineVShader = Shader::fromStr(source, type);

	}

	{
		const std::string source = R"(
struct PixelInput
{
    float4 color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET
{
    return input.color;
}
	)";

		const ShaderType type = ShaderType::Pixel;

		primitive2DPShader = Shader::fromStr(source, type);

	}

	{
		const std::string source = R"(
struct GeometryInput
{
    float4 pos : SV_Position;
    float width : POSITION1;
    float4 color : COLOR;
};

struct GeometryOutput
{
    float4 color : COLOR;
    float4 pos : SV_Position;
};

cbuffer Matrix2D : register(b0)
{
    matrix proj;
};

static const int triangleNum = 8;
    
static const float cosdTheta = cos(-3.1415926535 / (float) triangleNum);
static const float sindTheta = sin(-3.1415926535 / (float) triangleNum);

float2 rotTheta(float2 v)
{
    return float2(v.x * cosdTheta - v.y * sindTheta, v.x * sindTheta + v.y * cosdTheta);
}

[maxvertexcount(54)]
void main(
	line GeometryInput input[2],
	inout TriangleStream<GeometryOutput> outputs
)
{
    const float2 dir = input[1].pos.xy - input[0].pos.xy;
    const float2 dirN = normalize(dir);
    const float2 v = input[0].width / 2.0 * float2(-dirN.y, dirN.x);
    
    GeometryOutput rect[6];
    
    rect[0].color = input[0].color;
    rect[1].color = input[0].color;
    rect[2].color = input[0].color;
    rect[3].color = input[0].color;
    rect[4].color = input[0].color;
    rect[5].color = input[0].color;
    
    rect[0].pos = mul(input[0].pos + float4(v, 0.0, 0.0), proj);
    rect[1].pos = mul(input[1].pos + float4(v, 0.0, 0.0), proj);
    rect[2].pos = mul(input[0].pos - float4(v, 0.0, 0.0), proj);
    rect[3].pos = mul(input[1].pos + float4(v, 0.0, 0.0), proj);
    rect[4].pos = mul(input[1].pos - float4(v, 0.0, 0.0), proj);
    rect[5].pos = mul(input[0].pos - float4(v, 0.0, 0.0), proj);
    
    outputs.Append(rect[0]);
    outputs.Append(rect[1]);
    outputs.Append(rect[2]);
    outputs.Append(rect[3]);
    outputs.Append(rect[4]);
    outputs.Append(rect[5]);
    
    GeometryOutput output;
    output.color = input[0].color;
    
    float2 v0 = v;
    float2 v1 = rotTheta(v);
    
    [unroll]
    for (int i = 0; i < triangleNum-1; i++)
    {
        output.pos = mul(input[1].pos - float4(v, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[1].pos + float4(v0, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[1].pos + float4(v1, 0.0, 0.0), proj);
        outputs.Append(output);
        
        v0 = rotTheta(v0);
        v1 = rotTheta(v1);
    }
    
    v0 = rotTheta(v0);
    v1 = rotTheta(v1);
    
    [unroll]
    for (i = 0; i < triangleNum; i++)
    {
        output.pos = mul(input[0].pos + float4(v, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[0].pos + float4(v0, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[0].pos + float4(v1, 0.0, 0.0), proj);
        outputs.Append(output);
        
        v0 = rotTheta(v0);
        v1 = rotTheta(v1);
    }
    
}
	)";

		const ShaderType type = ShaderType::Geometry;

		rcLineGShader = Shader::fromStr(source, type);

	}

	{
		const std::string source = R"(
struct GeometryInput
{
	float4 pos : SV_Position;
	float4 color : COLOR;
};

struct GeometryOutput
{
    float4 color : COLOR;
    float4 pos : SV_Position;
};

cbuffer Matrix2D : register(b0)
{
    matrix proj;
};

cbuffer LineBuffer : register(b1)
{
	float lineWidth;
	float v1;
	float v2;
	float v3;
};

[maxvertexcount(6)]
void main(
	line GeometryInput input[2],
	inout TriangleStream<GeometryOutput> outputs
)
{
    const float2 dir = input[1].pos.xy - input[0].pos.xy;
    const float2 dirN = normalize(dir);
    const float2 v = lineWidth / 2.0 * float2(-dirN.y, dirN.x);
    
    GeometryOutput rect[6];
    
    rect[0].color = input[0].color;
    rect[1].color = input[0].color;
    rect[2].color = input[0].color;
    rect[3].color = input[0].color;
    rect[4].color = input[0].color;
    rect[5].color = input[0].color;
    
    rect[0].pos = mul(input[0].pos + float4(v, 0.0, 0.0), proj);
    rect[1].pos = mul(input[1].pos + float4(v, 0.0, 0.0), proj);
    rect[2].pos = mul(input[0].pos - float4(v, 0.0, 0.0), proj);
    rect[3].pos = mul(input[1].pos + float4(v, 0.0, 0.0), proj);
    rect[4].pos = mul(input[1].pos - float4(v, 0.0, 0.0), proj);
    rect[5].pos = mul(input[0].pos - float4(v, 0.0, 0.0), proj);
    
    outputs.Append(rect[0]);
    outputs.Append(rect[1]);
    outputs.Append(rect[2]);
    outputs.Append(rect[3]);
    outputs.Append(rect[4]);
    outputs.Append(rect[5]);
}
		)";

		const ShaderType type = ShaderType::Geometry;

		primitive2DGShader = Shader::fromStr(source, type);

	}

}

PrimitiveBatch* PrimitiveBatch::create()
{
	if (instance)
	{
		throw "[class PrimitiveBatch] Already have an instance!";
		return instance;
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
	delete primitive2DVShader;
	delete circleVShader;
	delete rcLineVShader;
	delete primitive2DPShader;
	delete primitive2DGShader;
	delete rcLineGShader;
}

void PrimitiveBatch::begin()
{
	lineRenderer.begin();
	circleRenderer.begin();
	rcLineRenderer.begin();
}

void PrimitiveBatch::end()
{
	Renderer::context->GSSetConstantBuffers(1, 1, lineBuffer.GetAddressOf());

	Renderer::context->IASetInputLayout(primitiveInputLayout.Get());
	primitive2DVShader->use();
	primitive2DPShader->use();
	primitive2DGShader->use();

	lineRenderer.end();

	Renderer::context->IASetInputLayout(circleInputLayout.Get());
	circleVShader->use();

	circleRenderer.end();

	Renderer::context->IASetInputLayout(rcLineInputLayout.Get());
	rcLineVShader->use();
	rcLineGShader->use();

	rcLineRenderer.end();

	Renderer::context->GSSetShader(nullptr, nullptr, 0);

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
	lineParam.lineWidth = width;
}

void PrimitiveBatch::applyChange() const
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(lineBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &lineParam, sizeof(LineParam));
	Renderer::context->Unmap(lineBuffer.Get(), 0);
}

PrimitiveBatch::LineRenderer::LineRenderer() :
	vertices(new float[2 * 6 * maxLineNum]), idx(0)
{
	//初始化vertexBuffer
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(float) * 2 * 6 * maxLineNum;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		Renderer::device->CreateBuffer(&vertexBufferDesc, nullptr, vertexBuffer.ReleaseAndGetAddressOf());
	}
}

PrimitiveBatch::LineRenderer::~LineRenderer()
{
	delete[] vertices;
}

void PrimitiveBatch::LineRenderer::begin()
{
	idx = 0;
}

void PrimitiveBatch::LineRenderer::end()
{
	updateVerticesData();

	unsigned int stride = sizeof(float) * 6;
	unsigned int offset = 0;
	Renderer::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Renderer::context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	Renderer::context->Draw(idx / 6, 0);
}

void PrimitiveBatch::LineRenderer::updateVerticesData() const
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, vertices, sizeof(float) * idx);
	Renderer::context->Unmap(vertexBuffer.Get(), 0);
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
	vertices(new float[maxCircleNum * 7]), idx(0)
{
	//初始化circleBuffer
	{
		float unitCircle[256];

		{
			float theta = 0.f;

			for (int i = 0; i < 64; i++)
			{
				unitCircle[i * 4] = cosf(theta);
				unitCircle[i * 4 + 1] = sinf(theta);
				unitCircle[i * 4 + 2] = cosf(theta + DirectX::XM_PI / 32.f);
				unitCircle[i * 4 + 3] = sinf(theta + DirectX::XM_PI / 32.f);
				theta += DirectX::XM_PI / 32.f;
			}
		}

		D3D11_BUFFER_DESC circleBufferDesc = {};
		circleBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
		circleBufferDesc.ByteWidth = sizeof(float) * 256;
		circleBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = {};
		subResource.pSysMem = unitCircle;

		Renderer::device->CreateBuffer(&circleBufferDesc, &subResource, circleBuffer.ReleaseAndGetAddressOf());
	}

	//初始化vertexBuffer
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(float) * 7 * maxCircleNum;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		Renderer::device->CreateBuffer(&vertexBufferDesc, nullptr, vertexBuffer.ReleaseAndGetAddressOf());
	}
}

PrimitiveBatch::CircleRenderer::~CircleRenderer()
{
	delete[] vertices;
}

void PrimitiveBatch::CircleRenderer::begin()
{
	idx = 0;
}

void PrimitiveBatch::CircleRenderer::end()
{
	updateVerticesData();

	unsigned int stride[2] = { sizeof(float) * 2u,sizeof(float) * 7u };
	unsigned int offset[2] = { 0,0 };

	ID3D11Buffer* buffers[2] = { circleBuffer.Get(),vertexBuffer.Get() };

	Renderer::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Renderer::context->IASetVertexBuffers(0, 2, buffers, stride, offset);

	Renderer::context->DrawInstanced(128, idx / 7, 0, 0);
}

void PrimitiveBatch::CircleRenderer::updateVerticesData() const
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, vertices, sizeof(float) * idx);
	Renderer::context->Unmap(vertexBuffer.Get(), 0);
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
	vertices(new float[maxLineNum * 7 * 2]), idx(0)
{
	//初始化vertexBuffer
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(float) * 2 * 7 * maxLineNum;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		Renderer::device->CreateBuffer(&vertexBufferDesc, nullptr, vertexBuffer.ReleaseAndGetAddressOf());
	}
}

PrimitiveBatch::RCLineRenderer::~RCLineRenderer()
{
	delete[] vertices;
}

void PrimitiveBatch::RCLineRenderer::begin()
{
	idx = 0;
}

void PrimitiveBatch::RCLineRenderer::end()
{
	updateVerticesData();

	unsigned int stride = sizeof(float) * 7;
	unsigned int offset = 0;

	Renderer::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Renderer::context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	Renderer::context->Draw(idx / 7, 0);
}

void PrimitiveBatch::RCLineRenderer::updateVerticesData() const
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, vertices, sizeof(float) * idx);
	Renderer::context->Unmap(vertexBuffer.Get(), 0);
}

void PrimitiveBatch::RCLineRenderer::addRoundCapLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& width, const float& r, const float& g, const float& b, const float& a)
{
	vertices[idx] = x1;
	vertices[idx + 1] = y1;
	vertices[idx + 2] = width;
	vertices[idx + 3] = r;
	vertices[idx + 4] = g;
	vertices[idx + 5] = b;
	vertices[idx + 6] = a;
	vertices[idx + 7] = x2;
	vertices[idx + 8] = y2;
	vertices[idx + 9] = width;
	vertices[idx + 10] = r;
	vertices[idx + 11] = g;
	vertices[idx + 12] = b;
	vertices[idx + 13] = a;

	idx += 14;
}
