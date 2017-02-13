#include <Framework/Mesh.h>

#include <GL/glew.h>
#include <iostream>

typedef struct Vertex
{
	float position[3];
	float normal[3];
	float tangent[3];
	float uv[2];
} Vertex;

Mesh::Mesh(unsigned const & numberOfFaces, aiFace * faces, aiVector3t<float> * positions, aiVector3t<float> * normals, aiVector3t<float> * tangents, aiVector3t<float> * textureCoords[8]) : m_vao(0), m_vbo(0), m_vertexCount(0)
{
	m_vertexCount = (numberOfFaces) * 3;

	Vertex * vertices = (Vertex*)malloc(sizeof(Vertex) * m_vertexCount);

	for (int i = 0; i < numberOfFaces; ++i)
	{
		for (int faceId = 0; faceId < 3; ++faceId)
		{
			aiVector3t<float> & position = positions[faces[i].mIndices[faceId]];
			aiVector3t<float> & normal = normals[faces[i].mIndices[faceId]];
			aiVector3t<float> & tangent = tangents[faces[i].mIndices[faceId]];

			vertices[(i * 3) + faceId].position[0] = positions[faces[i].mIndices[faceId]].x;
			vertices[(i * 3) + faceId].position[1] = positions[faces[i].mIndices[faceId]].y;
			vertices[(i * 3) + faceId].position[2] = positions[faces[i].mIndices[faceId]].z;
			vertices[(i * 3) + faceId].normal[0] = normals[faces[i].mIndices[faceId]].x;
			vertices[(i * 3) + faceId].normal[1] = normals[faces[i].mIndices[faceId]].y;
			vertices[(i * 3) + faceId].normal[2] = normals[faces[i].mIndices[faceId]].z;
			vertices[(i * 3) + faceId].tangent[0] = tangents[faces[i].mIndices[faceId]].x;
			vertices[(i * 3) + faceId].tangent[1] = tangents[faces[i].mIndices[faceId]].y;
			vertices[(i * 3) + faceId].tangent[2] = tangents[faces[i].mIndices[faceId]].z;
			vertices[(i * 3) + faceId].uv[0] = textureCoords[0][faces[i].mIndices[faceId]].x;
			vertices[(i * 3) + faceId].uv[1] = textureCoords[0][faces[i].mIndices[faceId]].y;
		}
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*m_vertexCount, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 9));

	free(vertices);
}


Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

unsigned const & Mesh::GetVAO() const
{
	return m_vao;
}

unsigned const & Mesh::GetVertexCount() const
{
	return m_vertexCount;
}