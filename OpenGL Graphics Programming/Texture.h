#pragma once
class Texture
{
public:
	Texture() = default;
	Texture(unsigned short _slot, float* _pixelArray);
	~Texture();

	void Bind() const;
	static void Unbind();

	unsigned int GetID() const { return m_textureID; }

private:
	unsigned int m_textureID{};
	int m_width{0};
	int m_height{0};

	//Texture bind slot, the size depends on the system, for 
	int m_slot{0};
};

