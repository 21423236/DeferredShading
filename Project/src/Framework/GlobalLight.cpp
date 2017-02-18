#include <Framework/GlobalLight.h>
#include <Framework/Defaults.h>
#include <GL/glew.h>

#pragma region "Constructors/Destructor"

GlobalLight::GlobalLight(std::string const & name, glm::vec3 const & intensity, glm::vec3 const & translation, glm::quat const & orientation) : Node(name, translation, orientation), m_intensity(intensity)
{
	CreateHandle();
}

GlobalLight::GlobalLight(std::string const & name, glm::vec3 const & intensity) : Node(name), m_intensity(intensity)
{
	CreateHandle();
}

GlobalLight::~GlobalLight()
{
	DestroyHandle();
}

#pragma endregion

#pragma region "Getters"

glm::vec3 const & GlobalLight::GetIntensity() const
{
	return m_intensity;
}

glm::mat4 const & GlobalLight::GetShadowMatrix() const
{
	return m_shadowMatrix;
}

unsigned int const & GlobalLight::GetShadowMap() const
{
	return m_shadowMap;
}

Node::NodeType GlobalLight::GetNodeType() const
{
	return Node::GLOBAL_LIGHT_NODE;
}

#pragma endregion

#pragma region "Setters"

void GlobalLight::SetIntensity(glm::vec3 const & intensity)
{
	m_intensity = intensity;
}

#pragma endregion

#pragma region "Private Methods"

void GlobalLight::CreateHandle()
{
	glGenTextures(1, &m_shadowMap);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, DEFAULT_SHADOW_WIDTH, DEFAULT_SHADOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GlobalLight::DestroyHandle()
{
	glDeleteTextures(1, &m_shadowMap);
}

#pragma endregion