#include "Mesh.h"



Mesh::Mesh(unsigned int _verticesSize, float* _vertices, unsigned int _indicesSize, const unsigned int* _indices)
{
	//Generate vao
	GLCall(glGenVertexArrays(1, &m_vaoID));
	GLCall(glBindVertexArray(m_vaoID));

	//Generate ibo
	GLCall(glGenBuffers(1, &m_iboID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indicesSize, _indices, GL_STATIC_DRAW));
	
	//Generate vbo
	GLCall(glGenBuffers(1, &m_vboID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vboID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, _verticesSize, _vertices, GL_STATIC_DRAW));

	m_indicesCount = _indicesSize;

	SetVertexAttributes();
	/*GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (GLvoid*)0));

	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (GLvoid*)(3 * sizeof(float))));

	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (GLvoid*)(7 * sizeof(float))));
	GLCall(glBindVertexArray(0));*/
}

void Mesh::AddVAOBuffer(const VertexBufferInfo & _info)
{
	const auto& indices = _info.Getindices();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < indices.size(); ++i)
	{
		const auto element = indices[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, (element.normalised ? GL_TRUE : GL_FALSE), _info.GetStride(), reinterpret_cast<const void*>(offset)));
		offset += element.count * VBElementInfo::GetTypeSize(element.type);
	}
}

void Mesh::SetVertexAttributes()
{
	//Create layout and populate it with the vertex attribute information
	VertexBufferInfo vbInfo;

	//3 position floats
	vbInfo.Push<float>(3);

	//4 colour floats
	vbInfo.Push<float>(4);

	//2 texture co-ord floats
	vbInfo.Push<float>(2);

	AddVAOBuffer(vbInfo);
}

void Mesh::Bind() const
{
	//Bind vao
	GLCall(glBindVertexArray(m_vaoID));
}

void Mesh::Unbind()
{
	GLCall(glBindVertexArray(0));
}

Mesh::~Mesh()
{
	GLCall(glDeleteBuffers(1, &m_vaoID));
	GLCall(glDeleteBuffers(1, &m_vboID));
	GLCall(glDeleteBuffers(1, &m_iboID));
}
