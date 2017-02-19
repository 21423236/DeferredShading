#include <Framework/UniformBuffer.h>
#include <Framework/Program.h>

#pragma region "Constructors/Destructor"

UniformBuffer::UniformBuffer(unsigned int const & blockIndex) : m_blockIndex(blockIndex), m_buffer(nullptr), m_bufferSize(0), m_uniforms(), m_handle(0)
{
}

UniformBuffer::~UniformBuffer()
{

}

#pragma endregion

#pragma region "Public Methods"

void UniformBuffer::AddUniform(std::string const & name, GLenum type)
{
	static int index = 0;

	struct Uniform uniform = { index++, GetStorageSize(type), m_bufferSize, type };
	m_bufferSize += uniform.size;
	m_uniforms[name] = uniform;
}

#define MIN(a, b) (a<b?a:b)
void UniformBuffer::SetUniform(std::string const & name, glm::mat4 const & matrix)
{
	if (m_uniforms.count(name))
	{
		Uniform const & uniform = m_uniforms[name];
		CopyUniform(&matrix[0][0], uniform.offset, MIN(uniform.size, sizeof(float) * 16));
	}
}
void UniformBuffer::SetUniform(std::string const & name, glm::mat3 const & matrix) 
{

}
void UniformBuffer::SetUniform(std::string const & name, glm::vec3 const & vector)
{

}
void UniformBuffer::SetUniform(std::string const & name, glm::vec2 const & vector)
{
	if (m_uniforms.count(name))
	{
		Uniform const & uniform = m_uniforms[name];
		CopyUniform(&vector[0], uniform.offset, MIN(uniform.size, sizeof(float) * 2));
	}
}
void UniformBuffer::SetUniform(std::string const & name, float const & value)
{

}
void UniformBuffer::SetUniform(std::string const & name, int const & value)
{

}
void UniformBuffer::SetUniform(std::string const & name, bool const & value)
{

}

void UniformBuffer::Initialize()
{
	m_buffer = (char*)malloc(m_bufferSize);
	memset(m_buffer, 0, m_bufferSize);

	glGenBuffers(1, &m_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, m_handle);
	glBufferData(GL_UNIFORM_BUFFER, m_bufferSize, &m_buffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_blockIndex, m_handle);
}

void UniformBuffer::UploadBuffer() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_handle);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, m_bufferSize, m_buffer);
}

void UniformBuffer::Free()
{
	free(m_buffer);
	glDeleteBuffers(1, &m_handle);
}

#pragma endregion

#pragma region "Private Methods"

#define CASE(Enum, Count, Type) if(type == Enum) return Count * sizeof(Type);
size_t UniformBuffer::GetStorageSize(GLenum const & type)
{
	CASE(GL_FLOAT, 1, GLfloat);
	CASE(GL_FLOAT_VEC2, 4, GLfloat);
	CASE(GL_FLOAT_VEC3, 3, GLfloat);
	CASE(GL_FLOAT_VEC4, 4, GLfloat);
	CASE(GL_INT, 1, GLint);
	CASE(GL_INT_VEC2, 2, GLint);
	CASE(GL_INT_VEC3, 3, GLint);
	CASE(GL_INT_VEC4, 4, GLint);
	CASE(GL_UNSIGNED_INT, 1, GLuint);
	CASE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
	CASE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
	CASE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
	CASE(GL_BOOL, 1, GLboolean);
	CASE(GL_BOOL_VEC2, 2, GLboolean);
	CASE(GL_BOOL_VEC3, 3, GLboolean);
	CASE(GL_BOOL_VEC4, 4, GLboolean);
	CASE(GL_FLOAT_MAT2, 4, GLfloat);
	CASE(GL_FLOAT_MAT2x3, 6, GLfloat);
	CASE(GL_FLOAT_MAT2x4, 8, GLfloat);
	CASE(GL_FLOAT_MAT3, 9, GLfloat);
	CASE(GL_FLOAT_MAT3x2, 6, GLfloat);
	CASE(GL_FLOAT_MAT3x4, 12, GLfloat);
	CASE(GL_FLOAT_MAT4, 16, GLfloat);
	CASE(GL_FLOAT_MAT4x2, 8, GLfloat);
	CASE(GL_FLOAT_MAT4x3, 12, GLfloat);
	return 0;
}

void UniformBuffer::CopyUniform(void const * source, GLint offset, GLint size)
{
	memcpy_s(m_buffer + offset, size, source, size);
}

#pragma endregion