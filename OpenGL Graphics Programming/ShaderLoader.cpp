#include "ShaderLoader.h" 
#include <fstream>
#include <vector>


GLuint ShaderLoader::CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	GLuint program = GLCall(glCreateProgram());

	const GLuint vertexID = CreateShader(GL_VERTEX_SHADER, vertexShaderFilename);
	const GLuint fragmentID = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);

	std::string ID = std::to_string(vertexID) + std::to_string(fragmentID);
	
	GLCall(glAttachShader(program, vertexID));
	GLCall(glAttachShader(program, fragmentID));

	glLinkProgram(program);

	const auto shader = m_shaderMap->find(program);

	if (shader != m_shaderMap->end() && shader->second == ID)
	{
		std::cout << "Shader has already been compiled before!" << std::endl;
		return shader->first;
	}

	// Check for link errors
	int link_result = 0;
	GLCall(glGetProgramiv(program, GL_LINK_STATUS, &link_result));
	if (link_result == GL_FALSE)
	{
		std::string programName = vertexShaderFilename + *fragmentShaderFilename;
		PrintErrorDetails(false, program, programName.c_str());
		return 0;
	}
	m_shaderMap->insert(std::pair<GLuint, std::string>(program, ID));
	return program;
}

GLuint ShaderLoader::CreateShader(GLenum shaderType, const char* shaderName)
{
	//Create Shader
	const GLuint shaderID = GLCall(glCreateShader(shaderType));

	std::string strShader = ReadShaderFile(shaderName);
	const char* cstrShader = strShader.c_str();

	GLCall(glShaderSource(shaderID, 1, &cstrShader, nullptr)); //Source string is already null terminating, so length can be nullptr

	GLCall(glCompileShader(shaderID));

	// Check for errors
	int compile_result = 0;
	GLCall(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_result));
	if (compile_result == GL_FALSE)
	{
		PrintErrorDetails(true, shaderID, shaderName);
		return 0;
	}
	return shaderID;
}

std::string ShaderLoader::ReadShaderFile(const char *filename)
{
	// Open the file for reading
	std::ifstream file(filename, std::ios::in);
	std::string shaderCode;

	// Ensure the file is open and readable
	if (!file.good()) {
		std::cout << "Cannot read file:  " << filename << std::endl;
		return "";
	}

	// Determine the size of of the file in characters and resize the string variable to accomodate
	file.seekg(0, std::ios::end);
	shaderCode.resize(static_cast<unsigned int>(file.tellg()));

	// Set the position of the next character to be read back to the beginning
	file.seekg(0, std::ios::beg);
	// Extract the contents of the file and store in the string variable
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}

void ShaderLoader::PrintErrorDetails(bool isShader, GLuint id, const char* name)
{
	int infoLogLength = 0;
	// Retrieve the length of characters needed to contain the info log
	(isShader) ? GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength)) : GLCall(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength));

	//Dynamically assign a char* string on the stack to be used for error message printing
	char* log = static_cast<char*>(alloca(static_cast<size_t>(infoLogLength * sizeof(char))));

	// Retrieve the log info and populate log variable
	(isShader) ? GLCall(glGetShaderInfoLog(id, infoLogLength, nullptr, log)) : GLCall(glGetProgramInfoLog(id, infoLogLength, nullptr, log));
	std::cout << "Error compiling " << ((isShader) ? "shader" : "program") << ": " << name << std::endl;
	std::cout << log << std::endl;
}