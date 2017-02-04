#include <Framework/Mesh.h>

#include <GL/glew.h>

#pragma region "Constructors/Destructor"

Mesh::Mesh(std::vector<glm::vec3> const & positions, std::vector<glm::vec3> const & normals, std::vector<glm::vec2> const & uvs)
{
	struct Vertex
	{
		float pos_x, pos_y, pos_z;
		float norm_x, norm_y, norm_z;
		float uv_x, uv_y;
	};

	std::vector<Vertex> vertices(positions.size());

	for (int i = 0; i < positions.size(); ++i)
	{
		vertices.push_back({ positions[i].x, positions[i].y, positions[i].z, normals[i].x, normals[i].y, normals[i].z, uvs[i].x, uvs[i].y });
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (GLvoid*)(sizeof(float) * 6));
}

Mesh::~Mesh()
{

}

#pragma endregion

#pragma region "Getters"

unsigned int const & Mesh::GetVertexCount() const
{
	return m_vertexCount;
}

#pragma endregion

#pragma region "Public Methods"

void Mesh::Use() const
{
	glBindVertexArray(m_vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Mesh::Unuse() const
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

#pragma endregion