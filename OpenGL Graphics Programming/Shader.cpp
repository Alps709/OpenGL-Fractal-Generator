#include "Shader.h"
#include "Debug.h"

#include <vector>
#include <freeglut.h>
#include <mat4x4.hpp>




unsigned int UniformGetLocation(const std::string& _name);

Shader::Shader()
= default;

Shader::Shader(const char* _vertexSrc, const char* _fragmentSrc)
{
	m_programID = ShaderLoader::CreateProgram(_vertexSrc, _fragmentSrc);
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_programID));
}

void Shader::Unbind()
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1f(const std::string& _name, float _v1) const
{
	GLCall(glUniform1f(GetUniformLocation(_name), _v1));
}

void Shader::SetUniform1i(const std::string& _name, int _v1) const
{
	GLCall(glUniform1i(GetUniformLocation(_name), _v1));
}

void Shader::SetUniformMat4f(const std::string& _name, glm::mat4& _matrix) const
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(_name), 1, false, &_matrix[0][0]));
}

void Shader::SetVertShaderName(std::string _name)
{
	m_vertShaderName = _name;
}

void Shader::SetFragShaderName(std::string _name)
{
	m_fragShaderName = _name;
}

int Shader::GetUniformLocation(const std::string& _name) const
{
	const int uniformLocation = GLCall(glGetUniformLocation(m_programID, _name.c_str()));

	if(uniformLocation == -1)
	{
		std::cout << "Warning! Couldn't find uniform: " << _name << std::endl;
	}

	return uniformLocation;
}
