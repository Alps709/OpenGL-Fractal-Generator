#include "Mesh.h"



Mesh::Mesh(std::vector<float> _vertices, std::vector<unsigned int> _indices)
{
	//Generate vao
	GLCall(glGenVertexArrays(1, &m_vaoID));
	GLCall(glBindVertexArray(m_vaoID));

	//Generate vbo
	GLCall(glGenBuffers(1, &m_vboID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vboID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(float), static_cast<const void*>(_vertices.data()), GL_STATIC_DRAW));

	//Generate ibo
	GLCall(glGenBuffers(1, &m_iboID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), static_cast<const void*>(_indices.data()), GL_STATIC_DRAW));

	m_indicesCount = _indices.size();

	SetVertexAttributes();
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
}
