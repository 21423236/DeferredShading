#include <Framework/Camera.h>



Camera::Camera(glm::mat4 & viewMatrix) : m_viewMatrix(viewMatrix)
{
}


Camera::~Camera()
{

}

//getters
glm::vec3 const & Camera::GetPosition() const
{
	return m_position;
}

float const & Camera::GetZoom() const
{
	return m_zoom;
}

float const & Camera::GetTilt() const
{
	return m_tilt;
}

float const & Camera::GetSpin() const
{
	return m_spin;
}

//setters
void Camera::SetPosition(float const & x, float const & y, float const & z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	UpdateViewMatrix();
}

void Camera::SetZoom(float const & zoom)
{
	m_zoom = zoom;
	UpdateViewMatrix();
}

void Camera::SetTilt(float const & tilt)
{
	m_tilt = tilt;
	UpdateViewMatrix();
}

void Camera::SetSpin(float const & spin)
{
	m_spin = spin;
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	float cosSpin = cosf(m_spin);
	float sinSpin = sinf(m_spin);
	float cosTilt = cosf(m_tilt);
	float sinTilt = sinf(m_tilt);
	m_viewMatrix = glm::mat4(
		glm::vec4(cosSpin, sinSpin * sinTilt, -sinSpin * cosTilt, 0), 
		glm::vec4(0, cosTilt, sinTilt, 0), 
		glm::vec4(sinSpin, -cosSpin * sinTilt, cosSpin * cosTilt, 0), 
		glm::vec4(-m_position.x, -m_position.y, -m_zoom, 1.0f));
}