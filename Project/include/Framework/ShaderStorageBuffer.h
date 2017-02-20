#pragma once

#include <vector>
#include <GL/glew.h>

template<class T>
class ShaderStorageBuffer
{
public:

	friend class DeferredRenderer;

	//constructors/destructor
	ShaderStorageBuffer(unsigned int const & binding, unsigned int sizeHint) : m_index(binding), m_buffer(sizeHint), m_bufferSize(sizeHint), m_handle(0)
	{

	}
	
	~ShaderStorageBuffer() 
	{
	}

	void Initialize()
	{
		glGenBuffers(1, &m_handle);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_handle);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T)*m_bufferSize, 0, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_index, m_handle);
	}

	void Upload()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_handle);
		if (m_buffer.size() > m_bufferSize)
		{
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T)*(m_bufferSize = m_buffer.size()), &m_buffer[0], GL_DYNAMIC_DRAW);
		}
		else
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(T)*m_buffer.size(), &m_buffer[0]);
		}
	}

	void Free()
	{
		glDeleteBuffers(1, &m_handle);
	}
	
private:

	GLuint m_index;
	std::vector<T> m_buffer;
	GLuint m_bufferSize;
	GLuint m_handle;

};

