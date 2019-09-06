#pragma once

// Dependency Includes
#include "Debug.h"


// Library Includes
#include <iostream>
#include <string>
#include <map>

static std::unique_ptr<std::map<GLuint, std::string>> m_shaderMap = std::make_unique<std::map <GLuint, std::string>>();

class ShaderLoader
{
	
private:
	ShaderLoader() { }

	//Make sure these don't do anything so you can't get any copies of the singleton
	ShaderLoader(ShaderLoader const&) = delete;          
	void operator=(ShaderLoader const&) = delete;

public:
	static ShaderLoader& GetInstance()
	{
		//Instantiated on first use
		static ShaderLoader instance; 
		return instance;
	}

	static GLuint CreateProgram(const char* VertexShaderFilename, const char* FragmentShaderFilename);

private:
	static GLuint CreateShader(GLenum shaderType, const char* shaderName);
	static std::string ReadShaderFile(const char *filename);
	static void PrintErrorDetails(bool isShader, GLuint id, const char* name);
};
