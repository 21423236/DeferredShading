#pragma once

#include <Framework/Node.h>

struct LocalLightInformation
{
	float position[4];
	float intensity[3];
	float radius;
};

class LocalLight : public Node
{
public:

	friend class GUI;

	//constructors/destructor
	LocalLight(std::string const & name, glm::vec3 const & intensity, float const & radius, glm::vec3 const & translation, glm::quat const & orientation);
	LocalLight(std::string const & name, glm::vec3 const & intensity, float const & radius);
	~LocalLight();

	//getters
	glm::vec3 const & GetIntensity() const;
	float const & GetRadius() const;
	NodeType GetNodeType() const;

	//setters
	void SetIntensity(glm::vec3 const & intensity);
	void SetRadius(float const & radius);

private:

	glm::vec3	m_intensity;
	float		m_radius;

};