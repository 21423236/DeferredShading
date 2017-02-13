#pragma once

#include "Node.h"

#include <glm/glm.hpp>

class Light : public Node
{
public:
	
	Light(glm::vec3 const & ambient, glm::vec3 const & intensity, bool const & isGlobal = true);
	~Light();

	//getters
	glm::vec3 const & GetAmbientIntensity() const;
	glm::vec3 const & GetIntensity() const;
	NodeType GetNodeType() const;

	//setters
	void SetAmbientIntensity(glm::vec3 const & ambient);
	void SetIntensity(glm::vec3 const & intensity);
	
	//public methods
	bool IsGlobal() const;


private:

	glm::vec3 m_ambient;
	glm::vec3 m_intensity;
	bool      m_isGlobal;
};

