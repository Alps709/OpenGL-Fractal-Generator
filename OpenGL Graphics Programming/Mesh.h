#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"

class Mesh
{
public:
	Mesh() = default;
	Mesh(unsigned int _verticesSize, float* _vertices, unsigned int _indicesSize, const unsigned int* _indices);
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

