#include "Texture.h"
#include "Debug.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <SOIL.h>

Texture::Texture(const char* _filePath, unsigned short _slot)
	: m_filePath(_filePath), m_slot(_slot)
{
	//Make sure slot is not higher than 16
	if (m_slot >= 16)
	{
		//Throw error
		std::string is = "ERROR: The texture slot shouldn't be 16 or over as certain systems do not have that many texture slots";
		std::cout << is << std::endl;
		throw std::logic_error(is.c_str());
	}
	
	GLCall(glGenTextures(1, &m_textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_textureID));

	unsigned char* tex1 = SOIL_load_image(m_filePath, &m_width, &m_height, nullptr, SOIL_LOAD_RGBA);
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1));

	GLCall(glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));

	SOIL_free_image_data(tex1);

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_textureID));
}

void Texture::Bind() const
{
	//Activates and binds texture to the specified slot
	GLCall(glActiveTexture(GL_TEXTURE0 + m_slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_textureID));
}

void Texture::Unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
