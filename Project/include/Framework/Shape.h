#pragma once

#include <vector>

class Shape
{
public:

	typedef enum {
		POINTS = 1,
		LINES = 2,
		LINE_LOOP = 3,
		TRIANGLES = 4,
		QUADS = 5
	} PrimitiveType;

	//static methods
	static Shape * GetFullScreenQuad();
	static Shape * GetIcosahedron();
	static Shape * GetWireCircle();
	static void FreeMemory();
	
	~Shape();

	//getters
	unsigned int const & GetVAO();
	unsigned int const & GetIndexCount();
	PrimitiveType const & GetPrimitiveType();

private:

	static void GenerateScreenQuad(Shape * & shape);
	static void GenerateIcosahedron(Shape * & shape);
	static void GenerateWireCircle(Shape * & shape);

	struct Vertex {
		float x, y, z;
	};

	struct Point {
		unsigned int a;
	};

	struct Line {
		unsigned int a, b;
	};

	struct LineLoop {
		unsigned int a;
	};

	struct Triangle {
		unsigned int a, b, c;
	};

	struct Quad {
		unsigned int a, b, c, d;
	};

	Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Point> const & indices);
	Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Line> const & indices);
	Shape(std::vector<struct Vertex> const & vertices, std::vector<struct LineLoop> const & indices);
	Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Triangle> const & indices);
	Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Quad> const & indices);

	//private methods
	void CreateHandles();
	void DestroyHandles();

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_indexCount;
	PrimitiveType m_primitiveType;

};

