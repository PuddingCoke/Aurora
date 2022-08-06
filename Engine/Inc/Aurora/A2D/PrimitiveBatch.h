#pragma once

#ifndef _PRIMITIVEBATCH_H_
#define _PRIMITIVEBATCH_H_

#include<Aurora/Shader.h>

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

private:

	static PrimitiveBatch* instance;

	PrimitiveBatch();

	void compileShaders();

	ComPtr<ID3D11InputLayout> primitiveInputLayout;
	ComPtr<ID3D11InputLayout> circleInputLayout;
	ComPtr<ID3D11InputLayout> rcLineInputLayout;

	Shader* primitive2DVShader;
	Shader* circleVShader;
	Shader* rcLineVShader;

	Shader* primitive2DPShader;

	Shader* primitive2DGShader;
	Shader* rcLineGShader;

	class LineRenderer
	{
	public:

		LineRenderer();

		~LineRenderer();

		void begin();

		void end();

		void updateVerticesData() const;

		void addLine(const float& x1, const float& y1, const float& x2, const float& y2, const float& r, const float& g, const float& b, const float& a);

		ComPtr<ID3D11Buffer> vertexBuffer;

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

		ComPtr<ID3D11Buffer> circleBuffer;

		ComPtr<ID3D11Buffer> vertexBuffer;

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

		ComPtr<ID3D11Buffer> vertexBuffer;

		float* vertices;

		int idx;

		static constexpr int maxLineNum = 150000;

	} rcLineRenderer;

};

#endif // !_PRIMITIVEBATCH_H_
