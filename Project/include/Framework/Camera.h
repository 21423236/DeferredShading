#pragma once

#include <glm/glm.hpp>

class Camera
{
public:

	Camera(glm::mat4 & viewMatrix);
	~Camera();

	//getters
	glm::vec3 const & GetPosition() const;
	float const & GetZoom() const;
	float const & GetTilt() const;
	float const & GetSpin() const;

	//setters
	void SetPosition(float const & x, float const & y, float const & z);
	void SetZoom(float const & zoom);
	void SetTilt(float const & tilt);
	void SetSpin(float const & spin);

private:

	void UpdateViewMatrix();

	glm::mat4 & m_viewMatrix;
	glm::vec3 m_position;
	float		m_zoom;
	float		m_tilt;
	float		m_spin;

};

