#pragma once

#include <glm/glm.hpp>

class Material
{
public:
	Material(glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha);
	~Material();

	//getters
	glm::vec3 const & GetKd() const;
	glm::vec3 const & GetKs() const;
	float const & GetAlpha() const;

	//setters
	void SetKd(glm::vec3 const & kd);
	void SetKs(glm::vec3 const & ks);
	void SetAlpha(float const & alpha);

private:
	glm::vec3 m_kd;
	glm::vec3 m_ks;
	float m_alpha;
};

