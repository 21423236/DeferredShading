#pragma once

#include <assimp/mesh.h>

class Mesh
{
public:

	Mesh(unsigned const & numberOfFaces, aiFace * faces, aiVector3t<float> * positions, aiVector3t<float> * normals, aiVector3t<float> * tangents, aiVector3t<float> * textureCoords[8]);
	~Mesh();

	unsigned const & GetVAO() const;
	unsigned const & GetVertexCount() const;

private:

	unsigned m_vao;
	unsigned m_vbo;
	unsigned m_vertexCount;

};

