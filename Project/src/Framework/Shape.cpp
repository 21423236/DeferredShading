#include <Framework/Shape.h>

#include <GL/glew.h>

#pragma region "Constructor/Destructor"

Shape::Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Point> const & indices) : m_vao(0), m_vbo(0), m_ibo(0), m_indexCount(indices.size()), m_primitiveType(POINTS)
{
	CreateHandles();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(struct Point)*indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}
Shape::Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Line> const & indices) : m_vao(0), m_vbo(0), m_ibo(0), m_indexCount(indices.size()), m_primitiveType(LINES)
{
	CreateHandles();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(struct Line)*indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}
Shape::Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Triangle> const & indices) : m_vao(0), m_vbo(0), m_ibo(0), m_indexCount(indices.size()), m_primitiveType(TRIANGLES)
{
	CreateHandles();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(struct Triangle)*indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}
Shape::Shape(std::vector<struct Vertex> const & vertices, std::vector<struct Quad> const & indices) : m_vao(0), m_vbo(0), m_ibo(0), m_indexCount(indices.size()), m_primitiveType(QUADS)
{
	CreateHandles();
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(struct Quad)*indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

Shape::~Shape()
{
	DestroyHandles();
}

#pragma endregion

#pragma region "Static Methods"

static Shape * g_fsq = nullptr;
static Shape * g_icosahedron = nullptr;

Shape * Shape::GetFullScreenQuad()
{
	if (!g_fsq)
		GenerateScreenQuad(g_fsq);
	return g_fsq;
}

Shape * Shape::GetIcosahedron()
{
	if (!g_icosahedron)
		GenerateIcosahedron(g_icosahedron);
	return g_icosahedron;
}

void Shape::GenerateScreenQuad(Shape * & shape)
{
	struct Vertex const vertices[4] = {
		{-1.0f, 1.0f, 0.0f},
		{-1.0f, -1.0f, 0.0f},
		{1.0f, -1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f}
	};

	struct Triangle const faces[2] = {
		{0, 1, 2},
		{2, 3, 0}
	};

	shape = new Shape(std::vector<struct Vertex>(vertices, vertices + 4), std::vector<struct Triangle>(faces, faces + 2));
}

void Shape::GenerateIcosahedron(Shape * & shape)
{
	float const t = (1.0f + sqrt(5.0f)) / 2.0f;

	struct Vertex const vertices[12] = {
		{ -1.0f, t, 0.0f },
		{ 1.0f, t, 0.0f },
		{ -1.0f, -t, 0.0f },
		{ 1.0f, -t, 0.0f },

		{ 0.0f, -1.0f, t },
		{ 0.0f, 1.0f, t },
		{ 0.0f, -1.0f, -t },
		{ 0.0f , 1.0f, -t },

		{ t, 0.0f, -1.0f },
		{ t, 0.0f, 1.0f },
		{ -t, 0.0f, -1.0f },
		{ -t, 0.0f, 1.0f }
	};

	struct Triangle const faces[20] = {
		{ 0, 11, 5 },
		{ 0, 5, 1 },
		{ 0, 1, 7 },
		{ 0, 7, 10 },
		{ 0, 10, 11 },

		{ 1, 5, 9 },
		{ 5, 11, 4 },
		{ 11, 10, 2 },
		{ 10, 7, 6 },
		{ 7, 1, 8 },

		{ 3, 9, 4 },
		{ 3, 4, 2 },
		{ 3, 2, 6 },
		{ 3, 6, 8 },
		{ 3, 8, 9 },

		{ 4, 9, 5 },
		{ 2, 4, 11 },
		{ 6, 2, 10 },
		{ 8, 6, 7 },
		{ 9, 8, 1 }
	};

	shape = new Shape(std::vector<struct Vertex>(vertices, vertices + 12), std::vector<struct Triangle>(faces, faces + 20));
}

void Shape::FreeMemory()
{
	if (g_fsq)
		delete g_fsq;
	if (g_icosahedron)
		delete g_icosahedron;
}

#pragma endregion

#pragma region "Getters"

unsigned int const & Shape::GetVAO()
{
	return m_vao;
}

unsigned int const & Shape::GetIndexCount()
{
	return m_indexCount;
}

Shape::PrimitiveType const & Shape::GetPrimitiveType()
{
	return m_primitiveType;
}

#pragma endregion

#pragma region "Private Methods"

void Shape::CreateHandles()
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
}

void Shape::DestroyHandles()
{
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

#pragma endregion