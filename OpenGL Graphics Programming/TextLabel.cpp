#include "TextLabel.h"

TextLabel::TextLabel(const std::string& _text, const std::string& _font, glm::vec2 _pos, glm::vec3 _colour, float _scale)
{
	m_text = _text;
	m_position = _pos;
	m_colour = _colour;
	m_scale = _scale;

	GLfloat halfWidth = static_cast<GLfloat>(inputManager.SCREEN_WIDTH) * 0.5f;
	GLfloat halfHeight = static_cast<GLfloat>(inputManager.SCREEN_HEIGHT) * 0.5f;

	m_projMat = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight);

	m_shader = Shader("Shaders/TextVertexShader.glsl", "Shaders/TextFragmentShader.glsl");

	FT_Library library;
	FT_Face fontFace;

	//Init the _font library
	if (FT_Init_FreeType(&library) != 0)
	{
		std::cout << "Freetype Lib failed init." << std::endl;
	}

	//Load the _font as face
	if (FT_New_Face(library, _font.c_str(), 0, &fontFace) != 0)
	{
		std::cout << "Freetype Face failed init." << std::endl;
	}

	FT_Set_Pixel_Sizes(fontFace, 0, 48); //set Size to load glyphs as
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	//disable byte alignment restriction

	//Loop 128 times to load 128 ascii characters
	for (GLubyte character = 0; character < 128; character++)
	{
		//Load the character glyph into the face
		if (FT_Load_Char(fontFace, character, FT_LOAD_RENDER))
		{
			std::cout << "Failed to load glyph (Ascii: " << character << ")" << std::endl;
		}

		//Generate a texture for each glyph
		GLuint texture = GenerateTexture(fontFace);

		FontChar fontChar = { texture,
								glm::ivec2(fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows),
								glm::ivec2(fontFace->glyph->bitmap_left, fontFace->glyph->bitmap_top),
								static_cast<GLuint>(fontFace->glyph->advance.x) };

		m_characters.insert(std::pair<GLchar, FontChar>(character, fontChar));
	}

	//Clean up the freetype stuff
	FT_Done_Face(fontFace);
	FT_Done_FreeType(library);

	//Generate vao
	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);

	//Generate vbo
	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	//Unbind stuff
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


}

void TextLabel::Render()
{
	//Bind shader and pass the uniforms
	m_shader.Bind();
	m_shader.SetUniform3f("u_textColour", m_colour.x, m_colour.y, m_colour.z);
	m_shader.SetUniformMat4f("u_proj", m_projMat);

	//Bind vao
	glBindVertexArray(m_vaoID);

	//Render each character of the m_text
	glm::vec2 textPos = m_position;
	for (std::string::const_iterator character = m_text.begin(); character != m_text.end(); ++character)
	{
		const FontChar currentChar = m_characters[*character];
		const GLfloat xPos = textPos.x + currentChar.Bearing.x * m_scale;
		const GLfloat yPos = textPos.y - static_cast<float>(currentChar.Size.y - currentChar.Bearing.y) * m_scale;
		const GLfloat charWidth = currentChar.Size.x * m_scale;
		const GLfloat charHeight = currentChar.Size.y * m_scale;

		//Generate the new vertices for the next character
		GLfloat vertices[6][4] =
		{
			{ xPos, yPos + charHeight, 0.0f, 0.0f },
			{ xPos, yPos, 0.0f, 1.0f },
			{ xPos + charWidth, yPos, 1.0f, 1.0f },
			{ xPos, yPos + charHeight, 0.0f, 0.0f },
			{ xPos + charWidth, yPos, 1.0f, 1.0f },
			{ xPos + charWidth, yPos + charHeight, 1.0f, 0.0f },
		};

		//Bind new vbo and substitute the old data with the new data
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		//Render the character
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, currentChar.TextureID);
		m_shader.SetUniform1i("u_tex", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		textPos.x += (currentChar.Advance >> 6) * m_scale;
	}

	//Unbind
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint TextLabel::GenerateTexture(FT_Face _face)
{
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
				 _face->glyph->bitmap.width,
				 _face->glyph->bitmap.rows,
				  0, GL_RED, GL_UNSIGNED_BYTE,
				 _face->glyph->bitmap.buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}
