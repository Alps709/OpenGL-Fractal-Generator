#pragma once
#include <string>
#include <mat4x4.hpp>
#include "ShaderLoader.h"

class Shader
{
public:
	Shader();
	Shader(const char* vertexSrc, const char* fragmentSrc);
	~Shader() = default;

	void Bind() const;
	static void Unbind();

	void SetUniform1f(const std::string& _name, float _v1) const;
	
	void SetUniform1i(const std::string& _name, int _v1) const;

	void SetUniformMat4f(const std::string& _name, glm::mat4& _matrix) const;

	void SetVertShaderName(std::string _name);
	void SetFragShaderName(std::string _name);

	int GetUniformLocation(const std::string& _name) const;

private:
	std::string m_vertShaderName;
	std::string m_fragShaderName;
	unsigned int m_programID{};
};

