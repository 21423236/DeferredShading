#include <Framework/Light.h>

Light::Light(glm::vec3 const & ambient, glm::vec3 const & intensity, bool const & isGlobal) : Node(), m_ambient(ambient), m_intensity(intensity), m_isGlobal(isGlobal)
{
}

Light::~Light()
{
}

glm::vec3 const & Light::GetAmbientIntensity() const
{
	return m_ambient;
}

glm::vec3 const & Light::GetIntensity() const
{
	return m_intensity;
}

void Light::SetAmbientIntensity(glm::vec3 const & ambient)
{
	m_ambient = ambient;
}

void Light::SetIntensity(glm::vec3 const & intensity)
{
	m_intensity = intensity;
}

Node::NodeType Light::GetNodeType() const
{
	return LIGHT_NODE;
}

bool Light::IsGlobal() const
{
	return m_isGlobal;
}