#pragma once

#include <glew.h>
#include <vector>
#include <cassert>

struct VBLayoutElement
{
	GLuint type;
	GLuint count;
	bool normalised;

	static GLuint GetTypeSize(GLuint _type)
	{
		switch (_type)
		{
			case GL_DOUBLE:       return 8;
			case GL_FLOAT:        return 4;
			case GL_UNSIGNED_INT: return 4;
		default: ;
		}
		assert(false);
		return 0;
	}
};

class VertexBufferLayout
{
public:
	VertexBufferLayout();
	~VertexBufferLayout();

	template<typename T>
	void Push(GLuint _count);

	template<>
	void Push<double>(GLuint _count);

	template<>
	void Push<float>(GLuint _count);

	template<>
	void Push<unsigned int>(GLuint _count);

	inline const std::vector<VBLayoutElement>& GetElements() const;

	inline GLuint GetStride() const;


private:
	std::vector <VBLayoutElement> m_Elements;
	GLuint m_Stride;
};

template <typename T>
void VertexBufferLayout::Push(GLuint _count)
{
	static_assert(false);
}

template <>
inline void VertexBufferLayout::Push<double>(GLuint _count)
{
	m_Elements.push_back({GL_DOUBLE, _count, false});
	m_Stride += _count * sizeof(GLdouble);
}

template <>
inline void VertexBufferLayout::Push<float>(GLuint _count)
{
	m_Elements.push_back({GL_FLOAT, _count, false});
	m_Stride += _count * sizeof(GLfloat);
}

template <>
inline void VertexBufferLayout::Push<unsigned>(GLuint _count)
{
	m_Elements.push_back({GL_UNSIGNED_INT, _count, false});
	m_Stride += _count * sizeof(GLuint);
}

inline const std::vector<VBLayoutElement>& VertexBufferLayout::GetElements() const
{
	return m_Elements;
}

inline GLuint VertexBufferLayout::GetStride() const
{
	return m_Stride;
}
