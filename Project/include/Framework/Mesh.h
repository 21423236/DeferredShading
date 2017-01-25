#pragma once

#include <vector>
#include <glm/glm.hpp>

class Mesh
{
public:

	//constructors/destructor
	Mesh(std::vector<glm::vec3> const & position, std::vector<glm::vec3> const & normals, std::vector<glm::vec2> const & uvs);
	~Mesh();

private:

	unsigned int m_vao;
	unsigned int m_vbo;

};

