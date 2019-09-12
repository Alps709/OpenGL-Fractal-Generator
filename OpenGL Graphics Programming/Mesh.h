#pragma once
#include "VertexBufferInfo.h"

class Mesh
{
public:
	Mesh() = default;
	Mesh(std::vector<float> _vertices, std::vector<unsigned int> _indices);
	~Mesh();

	void AddVAOBuffer(const VertexBufferInfo& _info);
	void SetVertexAttributes();

	unsigned int GetindicesCount() const { return m_indicesCount; }

	void Bind() const;
	static void Unbind();

private:
	GLuint m_vaoID{0};
	GLuint m_vboID{0};
	GLuint m_iboID{0};

	//Used by the renderer to know how many indices to draw
	unsigned int m_indicesCount{};
};

