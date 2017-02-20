#include <Framework/Material.h>

#pragma region "Constructors/Destructor"

Material::Material(glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha) : m_kd(kd), m_ks(ks), m_alpha(alpha), m_diffuseMap(nullptr), m_normalMap(nullptr), m_specularMap(nullptr)
{
}


Material::~Material()
{
}

#pragma endregion

#pragma region "Queries"

bool Material::HasDiffuseMap() const
{
	return m_diffuseMap != nullptr;
}

bool Material::HasNormalMap() const
{
	return m_normalMap != nullptr;
}

bool Material::HasSpecularMap() const
{
	return m_specularMap != nullptr;
}

#pragma endregion

#pragma region "Getters"

glm::vec3 & Material::GetKd()
{
	return m_kd;
}

glm::vec3 const & Material::GetKd() const
{
	return m_kd;
}

glm::vec3 & Material::GetKs()
{
	return m_ks;
}

glm::vec3 const & Material::GetKs() const
{
	return m_ks;
}

float & Material::GetAlpha()
{
	return m_alpha;
}

float const & Material::GetAlpha() const
{
	return m_alpha;
}

Texture const * const & Material::GetDiffuseMap() const
{
	return m_diffuseMap;
}

Texture const * const & Material::GetNormalMap() const
{
	return m_normalMap;
}

Texture const * const & Material::GetSpecularMap() const
{
	return m_specularMap;
}

#pragma endregion

#pragma region "Setters"

void Material::SetKd(glm::vec3 const & kd)
{
	m_kd = kd;
}

void Material::SetKs(glm::vec3 const & ks)
{
	m_ks = ks;
}

void Material::SetAlpha(float const & alpha)
{
	m_alpha = alpha;
}

void Material::SetDiffuseMap(Texture const * const & diffuseMap)
{
	m_diffuseMap = diffuseMap;
}

void Material::SetNormalMap(Texture const * const & normalMap)
{
	m_normalMap = normalMap;
}

void Material::SetSpecularMap(Texture const * const & specularMap)
{
	m_specularMap = specularMap;
}

#pragma endregion