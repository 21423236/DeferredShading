#include <Framework/Material.h>

Material::Material(glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha) : m_kd(kd), m_ks(ks), m_alpha(alpha)
{
}


Material::~Material()
{
}

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