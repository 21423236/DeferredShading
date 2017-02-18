#pragma once

#include <Framework/Node.h>

class GlobalLight : public Node
{
public:

	friend class GUI;
	friend class ShadowPass;
	
	//constructors/destructor
	GlobalLight(std::string const & name, glm::vec3 const & intensity, glm::vec3 const & translation, glm::quat const & orientation);
	GlobalLight(std::string const & name, glm::vec3 const & intensity);
	~GlobalLight();

	//getters
	glm::vec3 const &		GetIntensity() const;
	glm::mat4 const &		GetShadowMatrix() const;
	unsigned int const &	GetShadowMap() const;
	NodeType				GetNodeType() const;

	//setters
	void SetIntensity(glm::vec3 const & intensity);

private:

	//private methods
	void CreateHandle();
	void DestroyHandle();

	glm::vec3			m_intensity;
	mutable glm::mat4	m_shadowMatrix;
	unsigned int		m_shadowMap;
};

