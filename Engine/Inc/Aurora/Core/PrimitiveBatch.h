#pragma once

#ifndef _PRIMITIVEBATCH_H_
#define _PRIMITIVEBATCH_H_

#include<Aurora/EngineAPI/ImCtx.h>
#include<Aurora/Core/Shader.h>

#include<Aurora/CompiledShaders/PrimitiveBatchLineVS.h>
#include<Aurora/CompiledShaders/PrimitiveBatchCircleVS.h>
#include<Aurora/CompiledShaders/PrimitiveBatchRCLineVS.h>

#include<Aurora/CompiledShaders/PrimitiveBatchRCLineGS.h>
#include<Aurora/CompiledShaders/PrimitiveBatchLineGS.h>

#include<Aurora/CompiledShaders/PrimitiveBatchPS.h>

class PrimitiveBatch
{
public:

	static PrimitiveBatch* create();

	static PrimitiveBatch* get();

	PrimitiveBatch(const PrimitiveBatch&) = delete;

	void operator=(const PrimitiveBatch&) = delete;

	~PrimitiveBatch();

	void begin();

	void end();

	void drawLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& r, const float& g, const float& b, const float& a = 1.f);

	void drawCircle(const float& x, const float& y, const float& length, const float& r, const float& g, const float& b, const float& a = 1.f);

	void drawRoundCapLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& width, const float& r, const float& g, const float& b, const float& a = 1.f);

	void setLineWidth(const float& width);

	void applyChange() const;

private:

	static PrimitiveBatch* instance;

	PrimitiveBatch();

	void compileShaders();

	ComPtr<ID3D11InputLayout> primitiveInputLayout;
	ComPtr<ID3D11InputLayout> circleInputLayout;
	ComPtr<ID3D11InputLayout> rcLineInputLayout;

	ConstantBuffer* lineBuffer;

	struct LineParam
	{
		float lineWidth;
		float v0;
		float v1;
		float v2;
	}lineParam;

	Shader* lineVS;
	Shader* circleVS;
	Shader* rcLineVS;

	Shader* primitivePS;

	Shader* lineGS;

	Shader* rcLineGS;

	class LineRenderer
	{
	public:

		LineRenderer();

		~LineRenderer();

		void begin();

		void end();

		void updateVerticesData() const;

		void addLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& r, const float& g, const float& b, const float& a);

		VertexBuffer* vertexBuffer;

		float* vertices;

		int idx;

		static constexpr int maxLineNum = 150000;

	} lineRenderer;

	class CircleRenderer
	{
	public:

		CircleRenderer();

		~CircleRenderer();

		void begin();

		void end();

		void updateVerticesData() const;

		void addCircle(const float& x, const float& y, const float& length, const float& r, const float& g, const float& b, const float& a);

		VertexBuffer* vertexBuffer;

		float* vertices;

		static constexpr int maxCircleNum = 50000;

		int idx;

	}circleRenderer;

	class RCLineRenderer
	{
	public:

		RCLineRenderer();

		~RCLineRenderer();

		void begin();

		void end();

		void updateVerticesData() const;

		void addRoundCapLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& width, const float& r, const float& g, const float& b, const float& a);

		VertexBuffer* vertexBuffer;

		float* vertices;

		int idx;

		static constexpr int maxLineNum = 150000;

	} rcLineRenderer;

};

#endif // !_PRIMITIVEBATCH_H_
