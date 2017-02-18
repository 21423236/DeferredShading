#include <Framework/LocalLight.h>

#pragma region "Constructors/Destructor"

LocalLight::LocalLight(std::string const & name, glm::vec3 const & intensity, float const & radius, glm::vec3 const & translation, glm::quat const & orientation) : Node(name, translation, orientation), m_intensity(intensity), m_radius(radius)
{
}

LocalLight::LocalLight(std::string const & name, glm::vec3 const & intensity, float const & radius) : Node(name), m_intensity(intensity), m_radius(radius)
{
}

LocalLight::~LocalLight()
{
}

#pragma endregion

#pragma region "Getters"

glm::vec3 const & LocalLight::GetIntensity() const
{
	return m_intensity;
}

float const & LocalLight::GetRadius() const
{
	return m_radius;
}

Node::NodeType LocalLight::GetNodeType() const
{
	return Node::LOCAL_LIGHT_NODE;
}

#pragma endregion

#pragma region "Setters"

void LocalLight::SetIntensity(glm::vec3 const & intensity)
{
	m_intensity = intensity;
}

void LocalLight::SetRadius(float const & radius)
{
	m_radius = radius;
}

#pragma endregion