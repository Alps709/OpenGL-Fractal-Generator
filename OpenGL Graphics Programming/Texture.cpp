#include "Texture.h"
#include "Debug.h"
#include "Utils.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <SOIL.h>



Texture::Texture(unsigned short _slot, unsigned char* _pixelArray)
	:  m_slot(_slot)
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
	
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, Utils::SCREEN_WIDTH, Utils::SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixelArray));

	GLCall(glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	
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
