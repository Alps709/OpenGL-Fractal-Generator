#pragma once

//Dependencies
#include <glew.h>
#include <freeglut.h>
#include <SOIL.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

//Library includes
#include <map>
#include <string>
#include <iostream>
#include <utility>

//Local includes
#include "ShaderLoader.h"
#include "Shader.h"
#include "Math.h"
#include "InputManager.h"


struct FontChar
{
	GLuint     TextureID; 
	glm::ivec2 Size;	
	glm::ivec2 Bearing; 
	GLuint     Advance;		
};


class TextLabel
{
public:
	TextLabel
	(
		const std::string& _text, 
		const std::string& _font, 
		glm::vec2 _pos, 
		glm::vec3 _colour = glm::vec3(1.0f, 1.0f, 1.0f), 
		float _scale = 1.0f
	);

	~TextLabel() = default;

	void Render();
	void SetText(std::string _text) { m_text = _text; }
	void SetColor(glm::vec3 _colour) { m_colour = _colour; }
	void SetScale(GLfloat _scale) { m_scale = _scale; }
	void SetPosition(glm::vec2 _pos) { m_position = _pos; }

	std::string GetText() { return m_text; }

private:
	InputManager& inputManager = InputManager::getInstance();

	static GLuint GenerateTexture(FT_Face _face);

	std::string m_text{};
	glm::vec2 m_position{};
	glm::vec3 m_colour{};
	GLfloat m_scale{};
	
	GLuint m_vaoID{};
	GLuint m_vboID{};
	Shader m_shader;
	
	glm::mat4 m_projMat;
	std::map<GLchar, FontChar> m_characters;
};
