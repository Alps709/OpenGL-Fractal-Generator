#pragma once
class Texture
{
public:
	Texture() = default;
	Texture(const char* _filePath, unsigned short _slot);
	~Texture();

	void Bind() const;
	static void Unbind();

	unsigned int GetID() const { return m_textureID; }

private:
	unsigned int m_textureID{};
	const char* m_filePath{};
	int m_width{0};
	int m_height{0};

	//Texture bind slot, the size depends on the system, for 
	int m_slot{0};
};

