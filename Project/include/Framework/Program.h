#pragma once

#include <glm/glm.hpp>

class Program
{
public:

	typedef enum ShaderType {
		VERTEX_SHADER_TYPE = 0x8B31,
		FRAGMENT_SHADER_TYPE = 0x8B30
	} ShaderType;

	//constructors/destructor
	Program();
	~Program();

	//public methods
	void Use() const;
	void Unuse() const;

	void SetUniform(char const * name, glm::mat4 const & matrix) const;
	void SetUniform(char const * name, glm::mat3 const & matrix) const;
	void SetUniform(char const * name, glm::vec4 const & vector) const;
	void SetUniform(char const * name, glm::vec3 const & vector) const;
	void SetUniform(char const * name, glm::vec2 const & vector) const;
	void SetUniform(char const * name, float const & value) const;
	void SetUniform(char const * name, int const & value) const;

	void CreateHandle();
	void DestroyHandle();
	void AttachShader(ShaderType const & type, char const * path);
	void Link();


private:

	unsigned int m_handle;

};