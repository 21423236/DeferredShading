#include <Framework/GlobalLight.h>
#include <Framework/Defaults.h>
#include <GL/glew.h>

#pragma region "Constructors/Destructor"

GlobalLight::GlobalLight(std::string const & name, glm::vec3 const & intensity, glm::vec3 const & translation, glm::quat const & orientation) : Node(name, translation, orientation), m_intensity(intensity), m_shadowMatrix(1.0f), m_shadowMap(SHADOW_MAP_TEXTURE_UNIT, Texture::DEPTH)
{
	CreateHandle();
}

GlobalLight::GlobalLight(std::string const & name, glm::vec3 const & intensity) : Node(name), m_intensity(intensity), m_shadowMatrix(1.0f), m_shadowMap(SHADOW_MAP_TEXTURE_UNIT, Texture::DEPTH)
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

Texture const & GlobalLight::GetShadowMap() const
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
	m_shadowMap.Initialize(DEFAULT_SHADOW_WIDTH, DEFAULT_SHADOW_HEIGHT, GL_RGBA32F, GL_RGBA, GL_FLOAT, 0);
}

void GlobalLight::DestroyHandle()
{
	m_shadowMap.Free();
}

#pragma endregion