#pragma once

#include <vector>

class Shape
{
public:

	typedef enum {
		POINTS = 1,
		LINES = 2,
		TRIANGLES = 3,
		QUADS = 4
	} PrimitiveType;

	//static methods
	static Shape * GetFullScreenQuad();
	static Shape * GetIcosahedron();
	static void FreeMemory();
	
	~Shape();

	//getters
	unsigned int const & GetVAO();
	unsigned int const & GetIndexCount();
	PrimitiveType const & GetPrimitiveType();

private:

	static void GenerateScreenQuad(Shape * & shape);
	static void GenerateIcosahedron(Shape * & shape);

	struct Vertex {
		float x, y, z;
	};

	struct Point {
		int a;
	};

	struct Line {
		int a, b;
	};

	struct Triangle {
		int a, b, c;
	};

	struct Quad {
		int a, b, c, d;
	};

	Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Point> const & indices);
	Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Line> const & indices);
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

