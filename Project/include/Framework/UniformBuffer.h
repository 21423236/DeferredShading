#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <GL/glew.h>

#include <glm/glm.hpp>

class Program;

class UniformBuffer
{
public:

	//constructors/destructor
	UniformBuffer(unsigned int const & blockIndex);
	~UniformBuffer();

	void AddUniform(std::string const & name, GLenum type);

	void SetUniform(std::string const & name, glm::mat4 const & matrix);
	void SetUniform(std::string const & name, glm::mat3 const & matrix);
	void SetUniform(std::string const & name, glm::vec3 const & vector);
	void SetUniform(std::string const & name, glm::vec2 const & vector);
	void SetUniform(std::string const & name, float const & value);
	void SetUniform(std::string const & name, int const & value);
	void SetUniform(std::string const & name, bool const & value);

	void Initialize();

	void UploadBuffer() const;
	void Free();

private:

	static size_t GetStorageSize(GLenum const & type);
	void CopyUniform(void const * source, GLint offset, GLint size);

	struct Uniform {
		GLuint index;
		GLint size;
		GLint offset;
		GLenum type;
	};

	unsigned int m_blockIndex;
	char * m_buffer;
	size_t m_bufferSize;
	std::unordered_map<std::string, struct Uniform> m_uniforms;

	unsigned int m_handle;
};