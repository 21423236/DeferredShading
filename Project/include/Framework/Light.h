#pragma once

#include "Node.h"

#include <glm/glm.hpp>

class Light : public Node
{
public:

	friend class GUI;
	friend class ShadowPass;
	
	Light(std::string const & name, glm::vec3 const & ambient, glm::vec3 const & intensity, bool const & isGlobal = true);
	~Light();

	//getters
	glm::vec3 const & GetAmbientIntensity() const;
	glm::vec3 const & GetIntensity() const;
	unsigned int const & GetShadowTexture() const;
	glm::mat4 const & GetShadowMatrix() const;
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

	unsigned int m_shadowTexture;
	mutable glm::mat4 m_shadowMatrix;

};

