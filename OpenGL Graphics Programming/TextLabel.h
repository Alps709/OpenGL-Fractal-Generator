#pragma once

//Dependency includes
#include "Debug.h" //Has glew.h and iostream included
#include <glm.hpp>
#include <freeglut.h>
#include <SOIL.h>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

//Local includes
#include "Shader.h"
#include "ShaderLoader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"


struct FontChar
{
	GLuint      Texture2D;
	glm::ivec2  Size;
	glm::ivec2  Bearing;
	GLuint	    Advance;
};


class TextLabel
{
public:
	TextLabel
	(
		std::string text, 
		std::string font, 
		glm::vec2 pos, 
		glm::vec3 colour = glm::vec3(1.0f, 1.0f, 1.0f), 
		float scale = 1.0f
	);

	~TextLabel() = default;

	void Render();
	void SetText(std::string _newText) { m_text = _newText; }
	void SetColour(glm::vec3 _newColour) { m_colour = _newColour; }
	void SetScale(GLfloat _newScale) { m_scale = _newScale; }
	void SetPosition(glm::vec2 _newPosition) { m_position = _newPosition; }

private:

	GLuint GenerateTexture(FT_Face _face);

	std::string m_text;
	glm::vec3 m_colour;
	glm::vec2 m_position;
	float m_scale;

	VertexArray m_VAO;
	VertexBuffer m_VBO;
	Shader m_shader;

	glm::mat4 m_proj;
	std::map<GLchar, FontChar> m_characters;
};

