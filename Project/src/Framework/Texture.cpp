#include <Framework/Texture.h>
#include <GL/glew.h>

#pragma region "Constructors/Destructor"

Texture::Texture(unsigned int unit, DebugCorrectionType correction) : m_handle(0), m_unit(unit), m_correction(correction), m_width(0), m_height(0)
{

}

Texture::~Texture()
{

}

#pragma endregion

#pragma region "Public Methods"

void Texture::Initialize(unsigned int width, unsigned int height, unsigned int internalFormat, unsigned int format, unsigned int type, void * pixels)
{
	if (m_handle)
		glDeleteTextures(1, &m_handle);

	m_width = width;
	m_height = height;

	glGenTextures(1, &m_handle);
	glActiveTexture(m_unit);
	glBindTexture(GL_TEXTURE_2D, m_handle);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::Bind() const
{
	glActiveTexture(m_unit);
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

void Texture::Free()
{
	glDeleteTextures(1, &m_handle);
	m_handle = 0;
}

#pragma endregion

#pragma region "Getters"

unsigned int const & Texture::GetHandle() const
{
	return m_handle;
}

Texture::DebugCorrectionType const & Texture::GetCorrectionType() const
{
	return m_correction;
}

unsigned int const & Texture::GetWidth() const
{
	return m_width;
}

unsigned int const & Texture::GetHeight() const
{
	return m_height;
}

#pragma endregion