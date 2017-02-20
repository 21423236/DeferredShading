#pragma once

#include <glm/glm.hpp>
#include <Framework/Texture.h>

class Material
{
public:
	Material(glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha);
	~Material();

	//queries
	bool HasDiffuseMap() const;
	bool HasNormalMap() const;
	bool HasSpecularMap() const;

	//getters
	glm::vec3 & GetKd();
	glm::vec3 const & GetKd() const;
	glm::vec3 & GetKs();
	glm::vec3 const & GetKs() const;
	float & GetAlpha();
	float const & GetAlpha() const;
	Texture const * const & GetDiffuseMap() const;
	Texture const * const & GetNormalMap() const;
	Texture const * const & GetSpecularMap() const;

	//setters
	void SetKd(glm::vec3 const & kd);
	void SetKs(glm::vec3 const & ks);
	void SetAlpha(float const & alpha);
	void SetDiffuseMap(Texture const * const & diffuseMap);
	void SetNormalMap(Texture const * const & normalMap);
	void SetSpecularMap(Texture const * const & specularMap);

private:
	glm::vec3 m_kd;
	glm::vec3 m_ks;
	float m_alpha;

	Texture const * m_diffuseMap;
	Texture const * m_normalMap;
	Texture const * m_specularMap;

};

