#include <Framework/Light.h>
#include <Framework/Defaults.h>
#include <GL/glew.h>

Light::Light(std::string const & name, glm::vec3 const & ambient, glm::vec3 const & intensity, bool const & isGlobal) : Node(name), m_ambient(ambient), m_intensity(intensity), m_isGlobal(isGlobal), m_shadowTexture(0)
{
	if (m_isGlobal)
	{
		glGenTextures(1, &m_shadowTexture);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, m_shadowTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, DEFAULT_SHADOW_WIDTH, DEFAULT_SHADOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
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

unsigned int const & Light::GetShadowTexture() const
{
	return m_shadowTexture;
}

glm::mat4 const &  Light::GetShadowMatrix() const
{
	return m_shadowMatrix;
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